/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModelDelegate.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Maps/Commands/setMapCommand.hpp>
#include <BabelWiresLib/Maps/Commands/setMapSourceTypeCommand.hpp>
#include <BabelWiresLib/Maps/Commands/setMapTargetTypeCommand.hpp>
#include <BabelWiresLib/Maps/mapSerialization.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#define MAP_FILE_EXTENSION ".bw_map"
#define MAP_FORMAT_STRING "Map (*" MAP_FILE_EXTENSION ")"

babelwires::MapEditor::MapEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                                 const ComplexValueEditorData& data)
    : ComplexValueEditor(parent, projectBridge, userLogger, data)
    , m_map(projectBridge.getContext())
    , m_commandManager(m_map, userLogger) {
    setWindowTitle(getTitle());
    QLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    {
        auto contentsButtons = new QDialogButtonBox();
        {
            QPushButton* defaultButton = new QPushButton("Restore defaults");
            contentsButtons->addButton(defaultButton, QDialogButtonBox::ButtonRole::ResetRole);
            connect(defaultButton, &QAbstractButton::clicked, this, &MapEditor::setToDefault);
        }
        {
            QPushButton* saveButton =
                new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save copy to file");
            contentsButtons->addButton(saveButton, QDialogButtonBox::ButtonRole::AcceptRole);
            connect(saveButton, &QAbstractButton::clicked, this, &MapEditor::saveMapToFile);
        }
        {
            QPushButton* loadButton =
                new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "Load from file");
            contentsButtons->addButton(loadButton, QDialogButtonBox::ButtonRole::AcceptRole);
            connect(loadButton, &QAbstractButton::clicked, this, &MapEditor::loadMapFromFile);
        }
        mainLayout->addWidget(contentsButtons);
    }

    {
        QGroupBox* contents = new QGroupBox("Map contents", this);
        mainLayout->addWidget(contents);
        QLayout* contentsLayout = new QVBoxLayout();
        contents->setLayout(contentsLayout);

        QWidget* typeBar = new QWidget(this);
        contentsLayout->addWidget(typeBar);

        QLayout* typeBarLayout = new QHBoxLayout();
        typeBar->setLayout(typeBarLayout);

        {
            AccessModelScope scope(getProjectBridge());
            const UiProjectContext& context = projectBridge.getContext();
            const TypeSystem& typeSystem = context.m_typeSystem;
            const MapFeature& mapFeature = getMapFeature(scope);
            m_defaultMapValue = mapFeature.getDefaultMapData();
            const MapData& mapData = getMapDataFromProject(scope);
            MapFeature::AllowedTypes allowedTypeIds;
            mapFeature.getAllowedSourceTypeIds(allowedTypeIds);
            m_map.setAllowedSourceTypeId(allowedTypeIds);
            mapFeature.getAllowedTargetTypeIds(allowedTypeIds);
            m_map.setAllowedTargetTypeId(allowedTypeIds);
            m_map.setMapData(mapData);
            {
                typeBarLayout->addWidget(new QLabel("Source type: ", typeBar));
                m_sourceTypeWidget = new TypeWidget(typeBar, typeSystem, m_map.getAllowedSourceTypeIds());
                m_sourceTypeWidget->setTypeId(m_map.getSourceTypeRef());
                typeBarLayout->addWidget(m_sourceTypeWidget);
            }
            {
                typeBarLayout->addWidget(new QLabel("Target type: ", typeBar));
                m_targetTypeWidget = new TypeWidget(typeBar, typeSystem, m_map.getAllowedTargetTypeIds());
                m_targetTypeWidget->setTypeId(m_map.getTargetTypeRef());
                typeBarLayout->addWidget(m_targetTypeWidget);
            }
            connect(m_sourceTypeWidget, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                    [this]() { MapEditor::setSourceTypeFromWidget(); });
            connect(m_targetTypeWidget, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                    [this]() { MapEditor::setTargetTypeFromWidget(); });
        }
        m_mapView = new MapView;
        m_mapModel = new MapModel(m_mapView, *this);
        m_mapView->setContextMenuPolicy(Qt::CustomContextMenu);
        auto delegate = new MapModelDelegate(this);
        m_mapView->setItemDelegate(delegate);

        connect(m_mapView, SIGNAL(customContextMenuRequested(QPoint)), this,
                SLOT(onCustomContextMenuRequested(QPoint)));
        contentsLayout->addWidget(m_mapView);
        // connect(m_mapView, SIGNAL(clicked(const QModelIndex&)), m_mapModel, SLOT(onClicked(const QModelIndex&)));
        m_mapView->setModel(m_mapModel);
    }

    {
        auto bottomButtons = new QDialogButtonBox(QDialogButtonBox::Close);
        {
            QPushButton* refreshButton =
                new QPushButton(style()->standardIcon(QStyle::SP_BrowserReload), "Refresh from project");
            bottomButtons->addButton(refreshButton, QDialogButtonBox::ButtonRole::ResetRole);
            connect(refreshButton, &QAbstractButton::clicked, this, &MapEditor::updateMapFromProject);
        }
        {
            QPushButton* applyButton =
                new QPushButton(style()->standardIcon(QStyle::SP_DialogOkButton), "Apply to project");
            bottomButtons->addButton(applyButton, QDialogButtonBox::ButtonRole::ApplyRole);
            connect(applyButton, &QAbstractButton::clicked, this, &MapEditor::applyMapToProject);
        }
        connect(bottomButtons, &QDialogButtonBox::rejected, this, &MapEditor::close);

        mainLayout->addWidget(bottomButtons);
    }

    {
        m_undoAction = std::make_unique<QAction>(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
        m_undoAction->setShortcuts(QKeySequence::Undo);
        m_undoAction->setEnabled(false);
        m_undoAction->setShortcutContext(Qt::WindowShortcut);
        connect(m_undoAction.get(), &QAction::triggered, this, &MapEditor::undo);
        addAction(m_undoAction.get());

        m_redoAction = std::make_unique<QAction>(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
        m_redoAction->setShortcuts(QKeySequence::Redo);
        m_redoAction->setEnabled(false);
        m_redoAction->setShortcutContext(Qt::WindowShortcut);
        connect(m_redoAction.get(), &QAction::triggered, this, &MapEditor::redo);
        addAction(m_redoAction.get());

        m_undoStateChangedSubscription =
            m_commandManager.signal_undoStateChanged.subscribe([this]() { onUndoStateChanged(); });
    }

    show();
}

void babelwires::MapEditor::applyMapToProject() {
    auto modifierData = std::make_unique<MapValueAssignmentData>();
    modifierData->m_mapData = m_map.extractMapData();
    modifierData->m_pathToFeature = getData().getPathToValue();

    auto setValueCommand =
        std::make_unique<AddModifierCommand>("Set map value", getData().getElementId(), std::move(modifierData));
    if (!getProjectBridge().executeCommandSynchronously(std::move(setValueCommand))) {
        warnThatMapNoLongerInProject("Cannot apply the map.");
    } else {
        // Note: We take the position that once the map has been applied to the project, it's equivalent to
        // a document being saved in a document editor. When the map editor is closed, we check whether the
        // data was applied in the current state. We do not check that the value in the project still matches.
        m_commandManager.setCursor();
    }
}

const babelwires::MapFeature& babelwires::MapEditor::getMapFeature(AccessModelScope& scope) const {
    const ValueFeature& valueFeature = ComplexValueEditor::getValueFeature(scope, getData());
    const MapFeature* mapFeature = valueFeature.as<MapFeature>();
    assert(mapFeature && "The value feature was not a map feature");
    return *mapFeature;
}

const babelwires::MapData& babelwires::MapEditor::getMapDataFromProject(AccessModelScope& scope) const {
    const MapValueAssignmentData* const mapModifier = tryGetMapValueAssignmentData(scope);
    if (mapModifier) {
        return mapModifier->m_mapData;
    }
    return getMapFeature(scope).get();
}

const babelwires::MapFeature* babelwires::MapEditor::tryGetMapFeature(AccessModelScope& scope) const {
    const ValueFeature* valueFeature = ComplexValueEditor::tryGetValueFeature(scope, getData());
    if (valueFeature) {
        const MapFeature* mapFeature = valueFeature->as<MapFeature>();
        if (mapFeature) {
            return mapFeature;
        }
    }
    return nullptr;
}

const babelwires::MapValueAssignmentData*
babelwires::MapEditor::tryGetMapValueAssignmentData(AccessModelScope& scope) const {
    const FeatureElement* const element = scope.getProject().getFeatureElement(getData().getElementId());

    if (!element) {
        return nullptr;
    }

    const Modifier* const modifier = element->findModifier(getData().getPathToValue());

    if (!modifier) {
        return nullptr;
    }

    return modifier->getModifierData().as<MapValueAssignmentData>();
}

const babelwires::MapData* babelwires::MapEditor::tryGetMapDataFromProject(AccessModelScope& scope) const {
    const MapValueAssignmentData* const mapModifier = tryGetMapValueAssignmentData(scope);

    if (mapModifier) {
        return &mapModifier->m_mapData;
    }

    const babelwires::MapFeature* const mapFeature = tryGetMapFeature(scope);
    if (!mapFeature) {
        return nullptr;
    }

    return &mapFeature->get();
}

void babelwires::MapEditor::updateMapFromProject() {
    AccessModelScope scope(getProjectBridge());
    const MapData* mapDataFromProject = tryGetMapDataFromProject(scope);
    if (mapDataFromProject) {
        getUserLogger().logInfo() << "Refreshing the map from the project";
        executeCommand(
            std::make_unique<SetMapCommand>("Refresh the map from the project", mapDataFromProject->clone()));
    } else {
        warnThatMapNoLongerInProject("Cannot refresh the map.");
    }
}

void babelwires::MapEditor::setEditorMap(const MapData& map) {
    m_map.setMapData(map);
    updateUiAfterChange();
}

void babelwires::MapEditor::updateUiAfterChange() const {
    m_mapModel->valuesChanged();
    if (m_map.getSourceTypeValidity()) {
        m_sourceTypeWidget->removeBadItemIfPresent();
    } else {
        m_sourceTypeWidget->addBadItemIfNotPresent(m_map.getSourceTypeRef());
    }
    if (m_map.getTargetTypeValidity()) {
        m_targetTypeWidget->removeBadItemIfPresent();
    } else {
        m_targetTypeWidget->addBadItemIfNotPresent(m_map.getTargetTypeRef());
    }
    m_sourceTypeWidget->setTypeId(m_map.getSourceTypeRef());
    m_targetTypeWidget->setTypeId(m_map.getTargetTypeRef());
}

void babelwires::MapEditor::saveMapToFile() {
    QString dialogCaption = tr("Save project as");
    QString dialogFormats = tr("Map (*" MAP_FILE_EXTENSION ")");
    QString filePath = QFileDialog::getSaveFileName(this, dialogCaption, m_lastSaveFilePath, dialogFormats);
    if (!filePath.isNull()) {
        const QString ext = MAP_FILE_EXTENSION;
        if (!filePath.endsWith(ext)) {
            filePath += ext;
        }
        trySaveMapToFile(filePath);
        m_lastSaveFilePath = filePath;
    }
}

bool babelwires::MapEditor::trySaveMapToFile(const QString& filePath) {
    while (1) {
        try {
            std::string filePathStr = filePath.toStdString();
            getUserLogger().logInfo() << "Save map to \"" << filePathStr << '"';
            MapSerialization::saveToFile(filePathStr, m_map.extractMapData());
            return true;
        } catch (FileIoException& e) {
            getUserLogger().logError() << "The map could not be saved: " << e.what();
            QString message = e.what();
            if (QMessageBox::warning(this, tr("The map could not be saved."), message,
                                     QMessageBox::Retry | QMessageBox::Cancel,
                                     QMessageBox::Retry) == QMessageBox::Cancel) {
                return false;
            }
        }
    }
}

void babelwires::MapEditor::loadMapFromFile() {
    QString dialogCaption = tr("Load map from file");
    QString dialogFormats = tr(MAP_FORMAT_STRING);
    QString filePath = QFileDialog::getOpenFileName(this, dialogCaption, m_lastSaveFilePath, dialogFormats);
    if (!filePath.isNull()) {
        while (1) {
            try {
                std::string filePathStr = filePath.toStdString();
                getUserLogger().logInfo() << "Load map from \"" << filePathStr << '"';
                MapData mapData =
                    MapSerialization::loadFromFile(filePathStr, getProjectBridge().getContext(), getUserLogger());
                setEditorMap(mapData);
                m_lastSaveFilePath = filePath;
                return;
            } catch (FileIoException& e) {
                getUserLogger().logError() << "The map could not be loaded: " << e.what();
                QString message = e.what();
                if (QMessageBox::warning(this, tr("The map could not be loaded."), message,
                                         QMessageBox::Retry | QMessageBox::Cancel,
                                         QMessageBox::Retry) == QMessageBox::Cancel) {
                    return;
                }
            }
        }
    }
}

QString babelwires::MapEditor::getTitle() const {
    std::ostringstream contents;
    contents << getData() << " - Map Editor";
    return contents.str().c_str();
}

void babelwires::MapEditor::warnThatMapNoLongerInProject(const std::string& operationDescription) {
    std::ostringstream contents;
    contents << "The map " << getData() << " is no longer in the project.\n" << operationDescription;
    QMessageBox::warning(this, "Map no longer in project", QString(contents.str().c_str()));
}

void babelwires::MapEditor::onCustomContextMenuRequested(const QPoint& pos) {
    QModelIndex index = m_mapView->indexAt(pos);
    QMenu* const menu = m_mapModel->getContextMenu(index);
    if (menu) {
        menu->popup(m_mapView->mapToGlobal(pos));
    }
}

const babelwires::MapProject& babelwires::MapEditor::getMapProject() const {
    return m_map;
}

void babelwires::MapEditor::executeCommand(std::unique_ptr<Command<MapProject>> command) {
    if (m_commandManager.executeAndStealCommand(command)) {
        updateUiAfterChange();
    }
}

bool babelwires::MapEditor::maybeApplyToProject() {
    if (!m_commandManager.isAtCursor()) {
        while (1) {
            switch (QMessageBox::warning(
                this, tr("The map editor has unapplied changes."), tr("Do you want to apply them now?"),
                QMessageBox::Apply | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Apply)) {
                case QMessageBox::Apply:
                    applyMapToProject();
                    return true;
                case QMessageBox::Discard:
                    return true;
                default:
                    return false;
            }
        }
    }
    return true;
}

void babelwires::MapEditor::undo() {
    m_commandManager.undo();
    updateUiAfterChange();
}

void babelwires::MapEditor::redo() {
    m_commandManager.redo();
    updateUiAfterChange();
}

void babelwires::MapEditor::onUndoStateChanged() {
    if (m_commandManager.canUndo()) {
        m_undoAction->setEnabled(true);
        QString text = tr("Undo \"") + m_commandManager.getDescriptionOfUndoableCommand().c_str() + '"';
        m_undoAction->setText(text);
    } else {
        m_undoAction->setEnabled(false);
        m_undoAction->setText(tr("Undo"));
    }

    if (m_commandManager.canRedo()) {
        m_redoAction->setEnabled(true);
        QString text = tr("Redo \"") + m_commandManager.getDescriptionOfRedoableCommand().c_str() + '"';
        m_redoAction->setText(text);
    } else {
        m_redoAction->setEnabled(false);
        m_redoAction->setText(tr("Redo"));
    }
}

void babelwires::MapEditor::setToDefault() {
    executeCommand(std::make_unique<SetMapCommand>("Restore default map", m_defaultMapValue.clone()));
}

void babelwires::MapEditor::setSourceTypeFromWidget() {
    const TypeRef& newSourceTypeId = m_sourceTypeWidget->getTypeId();
    if (newSourceTypeId != m_map.getSourceTypeRef()) {
        executeCommand(std::make_unique<SetMapSourceTypeCommand>("Set map source type", newSourceTypeId));
    }
}

void babelwires::MapEditor::setTargetTypeFromWidget() {
    const TypeRef& newTargetTypeId = m_targetTypeWidget->getTypeId();
    if (newTargetTypeId != m_map.getTargetTypeRef()) {
        executeCommand(std::make_unique<SetMapTargetTypeCommand>("Set map target type", newTargetTypeId));
    }
}
