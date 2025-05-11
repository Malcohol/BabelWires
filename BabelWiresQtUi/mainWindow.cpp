/**
 * The MainWindow of the application.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/mainWindow.hpp>

#include <BabelWiresQtUi/LogWindow/logWindow.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphicsScene.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Commands/pasteNodesCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeNodeCommand.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Serialization/projectSerialization.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <Common/Log/unifiedLog.hpp>
#include <Common/exceptions.hpp>

#include <QtNodes/GraphicsView>

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QStyle>
#include <QToolBar>
#include <QtWidgets/QFileDialog>

namespace {
    constexpr char s_initialFilePath[] = "Untitled";

} // namespace

babelwires::MainWindow::MainWindow(ProjectGraphModel& projectGraphModel, UnifiedLog& log)
    : m_projectGraphModel(projectGraphModel)
    , m_userLogger(log) {

    m_graphicsScene = new ProjectGraphicsScene(projectGraphModel);
    // Don't call the constructor that takes a scene, since it calls setScene.
    auto view = new QtNodes::GraphicsView();
    // Don't call GraphicsView setScene, since it registers actions we don't want.
    view->QGraphicsView::setScene(m_graphicsScene);
    projectGraphModel.setWidgets(this, view);

    setCentralWidget(view);

    createActions();
    createMenus();
    createToolbars();
    createDockWidgets(log);

    setInitialFilePath();
    resize(800, 600);
    showNormal();
}

babelwires::MainWindow::~MainWindow() = default;

void babelwires::MainWindow::createActions() {
    m_newProjectAction = std::make_unique<QAction>(QIcon::fromTheme("document-new"), tr("&New project"), this);
    m_newProjectAction->setShortcuts(QKeySequence::New);
    m_newProjectAction->setStatusTip(tr("Start a new project"));
    m_newProjectAction->setEnabled(true);
    connect(m_newProjectAction.get(), &QAction::triggered, this, &MainWindow::newProject);

    m_openProjectAction = std::make_unique<QAction>(QIcon::fromTheme("document-open"), tr("&Open project\u2026"), this);
    m_openProjectAction->setShortcuts(QKeySequence::Open);
    m_openProjectAction->setStatusTip(tr("Open project"));
    m_openProjectAction->setEnabled(true);
    connect(m_openProjectAction.get(), &QAction::triggered, this, &MainWindow::openProject);

    m_saveProjectAction = std::make_unique<QAction>(QIcon::fromTheme("document-save"), tr("&Save project"), this);
    m_saveProjectAction->setShortcuts(QKeySequence::Save);
    m_saveProjectAction->setStatusTip(tr("Save project"));
    m_saveProjectAction->setEnabled(false);
    connect(m_saveProjectAction.get(), &QAction::triggered, this, &MainWindow::saveProject);

    m_saveProjectAsAction =
        std::make_unique<QAction>(QIcon::fromTheme("document-save-as"), tr("&Save project as\u2026"), this);
    m_saveProjectAsAction->setShortcuts(QKeySequence::SaveAs);
    m_saveProjectAsAction->setStatusTip(tr("Save project as"));
    m_saveProjectAsAction->setEnabled(true);
    connect(m_saveProjectAsAction.get(), &QAction::triggered, this, &MainWindow::saveProjectAs);

    m_exitAction = std::make_unique<QAction>(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setShortcuts(QKeySequence::Quit);
    m_exitAction->setEnabled(true);
    connect(m_exitAction.get(), &QAction::triggered, this, &MainWindow::close);

    m_undoAction = std::make_unique<QAction>(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    m_undoAction->setShortcuts(QKeySequence::Undo);
    m_undoAction->setEnabled(false);
    connect(m_undoAction.get(), &QAction::triggered, this, &MainWindow::undo);

    m_redoAction = std::make_unique<QAction>(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    m_redoAction->setShortcuts(QKeySequence::Redo);
    m_redoAction->setEnabled(false);
    connect(m_redoAction.get(), &QAction::triggered, this, &MainWindow::redo);

    {
        ModifyModelScope scope(m_projectGraphModel);
        m_undoStateChangedSubscription =
            scope.getCommandManager().signal_undoStateChanged.subscribe([this]() { onUndoStateChanged(); });
    }

    m_cutAction = std::make_unique<QAction>(QIcon::fromTheme("edit-cut"), tr("&Cut"), this);
    m_cutAction->setShortcuts(QKeySequence::Cut);
    m_cutAction->setEnabled(false);
    connect(m_cutAction.get(), &QAction::triggered, this, &MainWindow::cut);

    m_copyAction = std::make_unique<QAction>(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    m_copyAction->setShortcuts(QKeySequence::Copy);
    m_copyAction->setEnabled(false);
    connect(m_copyAction.get(), &QAction::triggered, this, &MainWindow::copy);

    connect(m_graphicsScene, &QGraphicsScene::selectionChanged, this, &MainWindow::onNodeSelectionChanged);
    
    m_pasteAction = std::make_unique<QAction>(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    m_pasteAction->setShortcuts(QKeySequence::Paste);
    onClipboardChanged();
    connect(m_pasteAction.get(), &QAction::triggered, this, &MainWindow::paste);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);

    m_showToolbarAction = std::make_unique<QAction>(tr("Show &toolbar"), this);
    m_showToolbarAction->setCheckable(true);
    m_showToolbarAction->setChecked(true);

    connect(m_showToolbarAction.get(), &QAction::toggled, this, &MainWindow::onShowMainToolbar);

    m_showLogWindow = std::make_unique<QAction>(tr("Show &log window"), this);
    m_showLogWindow->setCheckable(true);
    m_showLogWindow->setChecked(true);

    connect(m_showLogWindow.get(), &QAction::toggled, this, &MainWindow::onShowLogWindow);

    m_reloadAllSources =
        std::make_unique<QAction>(style()->standardIcon(QStyle::SP_BrowserReload), tr("&Reload all sources"), this);
    m_reloadAllSources->setEnabled(true);
    connect(m_reloadAllSources.get(), &QAction::triggered, this, &MainWindow::reloadAllSources);

    // TODO: Find or create a more suitable icon.
    // This is chosen because "save all targets" is conceptually like "running the project".
    m_saveAllTargets =
        std::make_unique<QAction>(style()->standardIcon(QStyle::SP_MediaPlay), tr("&Save all targets"), this);
    m_saveAllTargets->setEnabled(true);
    connect(m_saveAllTargets.get(), &QAction::triggered, this, &MainWindow::saveAllTargets);
}

void babelwires::MainWindow::createMenus() {
    {
        QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(m_newProjectAction.get());
        fileMenu->addAction(m_openProjectAction.get());
        fileMenu->addAction(m_saveProjectAction.get());
        fileMenu->addAction(m_saveProjectAsAction.get());
        fileMenu->addSeparator();
        fileMenu->addAction(m_exitAction.get());
    }
    {
        QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
        editMenu->addAction(m_undoAction.get());
        editMenu->addAction(m_redoAction.get());
        editMenu->addSeparator();
        editMenu->addAction(m_cutAction.get());
        editMenu->addAction(m_copyAction.get());
        editMenu->addAction(m_pasteAction.get());
    }
    {
        QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
        viewMenu->addAction(m_showToolbarAction.get());
        viewMenu->addAction(m_showLogWindow.get());
    }
    {
        QMenu* projectMenu = menuBar()->addMenu(tr("&Project"));
        projectMenu->addAction(m_reloadAllSources.get());
        projectMenu->addAction(m_saveAllTargets.get());
    }
}

void babelwires::MainWindow::createToolbars() {
    m_mainToolbar = new QToolBar("Main toolbar", this);

    m_mainToolbar->addAction(m_newProjectAction.get());
    m_mainToolbar->addAction(m_openProjectAction.get());
    m_mainToolbar->addAction(m_saveProjectAction.get());

    m_mainToolbar->addSeparator();

    m_mainToolbar->addAction(m_undoAction.get());
    m_mainToolbar->addAction(m_redoAction.get());

    m_mainToolbar->addSeparator();

    m_mainToolbar->addAction(m_copyAction.get());
    m_mainToolbar->addAction(m_cutAction.get());
    m_mainToolbar->addAction(m_pasteAction.get());

    m_mainToolbar->addSeparator();

    m_mainToolbar->addAction(m_reloadAllSources.get());
    m_mainToolbar->addAction(m_saveAllTargets.get());

    addToolBar(m_mainToolbar);
}

void babelwires::MainWindow::createDockWidgets(UnifiedLog& log) {
    m_logWindow = new LogWindow(this, log);
    addDockWidget(Qt::BottomDockWidgetArea, m_logWindow);
    connect(m_logWindow, &QDockWidget::visibilityChanged, m_showLogWindow.get(), &QAction::setChecked);
}

void babelwires::MainWindow::onUndoStateChanged() {
    AccessModelScope scope(m_projectGraphModel);
    const babelwires::CommandManager<Project>& commandManager = scope.getCommandManager();

    if (commandManager.canUndo()) {
        m_undoAction->setEnabled(true);
        QString text = tr("Undo \"") + commandManager.getDescriptionOfUndoableCommand().c_str() + '"';
        m_undoAction->setText(text);
    } else {
        m_undoAction->setEnabled(false);
        m_undoAction->setText(tr("Undo"));
    }

    if (commandManager.canRedo()) {
        m_redoAction->setEnabled(true);
        QString text = tr("Redo \"") + commandManager.getDescriptionOfRedoableCommand().c_str() + '"';
        m_redoAction->setText(text);
    } else {
        m_redoAction->setEnabled(false);
        m_redoAction->setText(tr("Redo"));
    }

    const bool isModified = !commandManager.isAtCursor();
    m_saveProjectAction->setEnabled(isModified);
    setWindowModified(isModified);
}

void babelwires::MainWindow::openProject() {
    if (maybeSave()) {
        QString dialogCaption = tr("Open project");
        QString filePath = QFileDialog::getOpenFileName(m_projectGraphModel.getFlowGraphWidget(), dialogCaption,
                                                        m_currentProjectDir, getDialogProjectFormat());
        if (!filePath.isNull()) {
            {
                logDebug() << "Clearing project before open project";
                ModifyModelScope scope(m_projectGraphModel);
                scope.getProject().clear();
                scope.getCommandManager().clear();
                setInitialFilePath();
            }
            while (1) {
                try {
                    std::string filePathStr = filePath.toStdString();
                    m_userLogger.logInfo() << "Open project \"" << filePathStr << '"';
                    ModifyModelScope scope(m_projectGraphModel);
                    ProjectData projectData =
                        ProjectSerialization::loadFromFile(filePathStr, m_projectGraphModel.getContext(), m_userLogger);
                    scope.getProject().setProjectData(projectData);
                    setCurrentFilePath(filePath);
                    return;
                } catch (FileIoException& e) {
                    m_userLogger.logError() << "The project could not be opened: " << e.what();
                    QString message = e.what();
                    if (QMessageBox::warning(this, tr("The project could not be opened."), message,
                                             QMessageBox::Retry | QMessageBox::Cancel,
                                             QMessageBox::Retry) == QMessageBox::Cancel) {
                        return;
                    }
                }
            }
        }
    }
}

bool babelwires::MainWindow::trySaveProject(const QString& filePath) {
    while (1) {
        try {
            std::string filePathStr = filePath.toStdString();
            m_userLogger.logInfo() << "Save project to \"" << filePathStr << '"';
            ModifyModelScope scope(m_projectGraphModel);
            ProjectSerialization::saveToFile(filePathStr, scope.getProject().extractProjectData());
            scope.getCommandManager().setCursor();
            return true;
        } catch (FileIoException& e) {
            m_userLogger.logError() << "The project could not be saved: " << e.what();
            QString message = e.what();
            if (QMessageBox::warning(this, tr("The project could not be saved."), message,
                                     QMessageBox::Retry | QMessageBox::Cancel,
                                     QMessageBox::Retry) == QMessageBox::Cancel) {
                return false;
            }
        }
    }
}

QString babelwires::MainWindow::getFullFilePath() const {
    if (!m_currentProjectFileName.isEmpty()) {
        return QFileInfo(m_currentProjectDir + "/" + m_currentProjectFileName).filePath();
    } else {
        return QString();
    }
}

void babelwires::MainWindow::saveProject() {
    if (!m_currentProjectFileName.isEmpty()) {
        trySaveProject(getFullFilePath());
    } else {
        saveProjectAs();
    }
}

void babelwires::MainWindow::saveProjectAs() {
    QString dialogCaption = tr("Save project as");
    QString filePath = QFileDialog::getSaveFileName(m_projectGraphModel.getFlowGraphWidget(), dialogCaption,
                                                    m_currentProjectDir, getDialogProjectFormat());
    if (!filePath.isNull()) {
        const QString ext = getProjectExtension();
        if (!filePath.endsWith(ext)) {
            filePath += ext;
        }
        trySaveProject(filePath);
        setCurrentFilePath(filePath);
    }
}

void babelwires::MainWindow::newProject() {
    if (maybeSave()) {
        m_userLogger.logInfo() << "New project";
        ModifyModelScope scope(m_projectGraphModel);
        scope.getProject().clear();
        scope.getCommandManager().clear();
        setInitialFilePath();
    }
}

void babelwires::MainWindow::undo() {
    ModifyModelScope scope(m_projectGraphModel);
    scope.getCommandManager().undo();
}

void babelwires::MainWindow::redo() {
    ModifyModelScope scope(m_projectGraphModel);
    scope.getCommandManager().redo();
}

babelwires::ProjectData babelwires::MainWindow::getProjectDataFromSelection() {
    ProjectData projectData = m_projectGraphModel.getDataFromSelectedNodes(m_graphicsScene->selectedItems());
    
    auto* flowView = dynamic_cast<QtNodes::GraphicsView*>(centralWidget());
    assert(flowView && "Unexpected central widget");
    const QPointF centre = flowView->sceneRect().center();
    UiPosition offset{static_cast<UiCoord>(-centre.x()), static_cast<UiCoord>(-centre.y())};
    projectUtilities::translate(offset, projectData);
    
    return projectData;
}

void babelwires::MainWindow::writeToClipboard(ProjectData projectData) {
    std::string asString = ProjectSerialization::saveToString(getFullFilePath().toStdString(), std::move(projectData));

    QByteArray contents(asString.c_str(), static_cast<int>(asString.size()));
    auto mimedata = std::make_unique<QMimeData>();
    mimedata->setData(getClipboardMimetype(), contents);
    QApplication::clipboard()->setMimeData(mimedata.release());
}

void babelwires::MainWindow::cut() {
    auto projectData = getProjectDataFromSelection();

    auto command = std::make_unique<RemoveNodeCommand>("Cut elements");
    for (const auto& node : projectData.m_nodes) {
        command->addNodeToRemove(node->m_id);
    }

    writeToClipboard(std::move(projectData));

    m_projectGraphModel.scheduleCommand(std::move(command));
}

void babelwires::MainWindow::copy() {
    // There's no associated command, so log explicitly.
    m_userLogger.logInfo() << "Copy selection";
    writeToClipboard(getProjectDataFromSelection());
}

void babelwires::MainWindow::paste() {
    const QMimeData* mimedata = QApplication::clipboard()->mimeData();
    QByteArray contents = mimedata->data(getClipboardMimetype());
    std::string asString(contents.data(), contents.length());

    try {
        ProjectData projectData = ProjectSerialization::loadFromString(asString, m_projectGraphModel.getContext(),
                                                                       getFullFilePath().toStdString(), m_userLogger);
        {
            auto* flowView = dynamic_cast<QtNodes::GraphicsView*>(centralWidget());
            assert(flowView && "Unexpected central widget");
            const QPointF centre = flowView->sceneRect().center();
            UiPosition offset{static_cast<UiCoord>(centre.x()), static_cast<UiCoord>(centre.y())};
            projectUtilities::translate(offset, projectData);
        }

        auto command = std::make_unique<PasteNodesCommand>("Paste elements", std::move(projectData));
        m_projectGraphModel.scheduleCommand(std::move(command));
    } catch (std::exception&) {
        // When using a specific mime-type, log a debug message?
        m_userLogger.logWarning() << "Failed to paste from clipboard";
    }
    // TODO Selection
    //m_projectGraphModel.selectNewNodes();
}

bool babelwires::MainWindow::maybeSave() {
    AccessModelScope scope(m_projectGraphModel);
    if (!scope.getCommandManager().isAtCursor()) {
        while (1) {
            switch (QMessageBox::warning(
                this, tr("The project has unsaved changes."), tr("Do you want to save them now?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save)) {
                case QMessageBox::Save:
                    if (trySaveProject(getFullFilePath())) {
                        return true;
                    }
                    break;
                case QMessageBox::Discard:
                    return true;
                default:
                    return false;
            }
        }
    }
    return true;
}

void babelwires::MainWindow::setCurrentFilePath(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    m_currentProjectDir = fileInfo.canonicalPath();
    m_currentProjectFileName = fileInfo.fileName();
    QString title = m_currentProjectFileName + "[*] \u2014 " + getApplicationTitle();
    setWindowTitle(title);
}

void babelwires::MainWindow::setInitialFilePath() {
    // Leave the directory intact.
    m_currentProjectFileName.clear();
    QString title = tr(s_initialFilePath) + "[*] \u2014 " + getApplicationTitle();
    setWindowTitle(title);
}

void babelwires::MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        m_userLogger.logInfo() << "Close";
        event->accept();
        m_valueEditorManager.closeAllValueEditors();
    } else {
        event->ignore();
    }
}

void babelwires::MainWindow::reloadAllSources() {
    m_userLogger.logInfo() << "Reload all sources";
    ModifyModelScope scope(m_projectGraphModel);
    scope.getProject().tryToReloadAllSources();
}

void babelwires::MainWindow::saveAllTargets() {
    m_userLogger.logInfo() << "Save all targets";
    ModifyModelScope scope(m_projectGraphModel);
    scope.getProject().tryToSaveAllTargets();
}

void babelwires::MainWindow::onNodeSelectionChanged() {
    const bool enabled = (m_graphicsScene->selectedItems().size() > 0);
    m_cutAction->setEnabled(enabled);
    m_copyAction->setEnabled(enabled);
}

void babelwires::MainWindow::onClipboardChanged() {
    m_pasteAction->setEnabled(QApplication::clipboard()->mimeData()->hasFormat(getClipboardMimetype()));
}

void babelwires::MainWindow::onShowMainToolbar(bool show) {
    m_mainToolbar->setHidden(!show);
}

void babelwires::MainWindow::onShowLogWindow(bool show) {
    m_logWindow->setHidden(!show);
}

QString babelwires::MainWindow::getApplicationTitle() const {
    const std::string& title = m_projectGraphModel.getContext().m_applicationIdentity.m_applicationTitle;
    assert(!title.empty() && "m_applicationTitle must be provided");
    return title.c_str();
}

QString babelwires::MainWindow::getProjectExtension() const {
    const std::string& ext = m_projectGraphModel.getContext().m_applicationIdentity.m_projectExtension;
    assert(!ext.empty() && "m_projectExtension must be provided");
    assert((ext[0] == '.') && "The extension must begin with .");
    return ext.c_str();
}

QString babelwires::MainWindow::getDialogProjectFormat() const {
    return QString("%1 project (*%2)").arg(getApplicationTitle(), getProjectExtension());
}

QString babelwires::MainWindow::getClipboardMimetype() const {
    // This doesn't need to be application specific.
    return QString("BabelWires/elements");
}

void babelwires::MainWindow::openEditorForValue(const ProjectDataLocation& data) {
    m_valueEditorManager.openEditorForValue(this, m_projectGraphModel, m_userLogger, data);
}
