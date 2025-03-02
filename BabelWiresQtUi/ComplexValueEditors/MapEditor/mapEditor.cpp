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

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>
#include <BabelWiresLib/Types/Map/Commands/setMapSourceTypeCommand.hpp>
#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapSerialization.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

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
                                 const ProjectDataLocation& data)
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
            const ValueTreeNode& mapTreeNode = getMapTreeNode(scope);
            m_typeRef = mapTreeNode.getTypeRef();
            const MapValue& mapValue = getMapValueFromProject(scope);
            if (mapTreeNode.getType().as<MapType>()) {
                m_map.setAllowedSourceTypeRefs(MapProject::AllowedTypes{{mapTreeNode.getType().is<MapType>().getSourceTypeRef()}});
                m_map.setAllowedTargetTypeRefs(MapProject::AllowedTypes{{mapTreeNode.getType().is<MapType>().getTargetTypeRef()}});
            } else {
                const SumOfMapsType *const sumOfMaps = mapTreeNode.getType().as<SumOfMapsType>();
                assert(sumOfMaps && "MapEditor expecting a MapType of SumOfMapsType");
                m_map.setAllowedSourceTypeRefs(MapProject::AllowedTypes{sumOfMaps->getSourceTypes(), sumOfMaps->getIndexOfDefaultSourceType()});
                m_map.setAllowedTargetTypeRefs(MapProject::AllowedTypes{sumOfMaps->getTargetTypes(), sumOfMaps->getIndexOfDefaultTargetType()});
            }
            m_map.setMapValue(mapValue);
            {
                typeBarLayout->addWidget(new QLabel("Source type: ", typeBar));
                m_sourceTypeWidget = new TypeWidget(typeBar, typeSystem, m_map.getAllowedSourceTypeRefs());
                m_sourceTypeWidget->setTypeRef(m_map.getCurrentSourceTypeRef());
                typeBarLayout->addWidget(m_sourceTypeWidget);
            }
            {
                typeBarLayout->addWidget(new QLabel("Target type: ", typeBar));
                m_targetTypeWidget = new TypeWidget(typeBar, typeSystem, m_map.getAllowedTargetTypeRefs());
                m_targetTypeWidget->setTypeRef(m_map.getCurrentTargetTypeRef());
                typeBarLayout->addWidget(m_targetTypeWidget);
            }
            connect(m_sourceTypeWidget, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                    [this]() { MapEditor::setSourceTypeFromWidget(); });
            connect(m_targetTypeWidget, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                    [this]() { MapEditor::setTargetTypeFromWidget(); });
        }
        m_mapView = new MapView;
        m_mapModel = new MapModel(m_mapView, projectBridge.getContext(), *this);
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
    auto modifierData = std::make_unique<ValueAssignmentData>(m_map.extractMapValue());
    modifierData->m_targetPath = getDataLocation().getPathToValue();

    auto setValueCommand =
        std::make_unique<AddModifierCommand>("Set map value", getDataLocation().getNodeId(), std::move(modifierData));
    if (!getProjectBridge().executeCommandSynchronously(std::move(setValueCommand))) {
        warnThatMapNoLongerInProject("Cannot apply the map.");
    } else {
        // Note: We take the position that once the map has been applied to the project, it's equivalent to
        // a document being saved in a document editor. When the map editor is closed, we check whether the
        // data was applied in the current state. We do not check that the value in the project still matches.
        m_commandManager.setCursor();
    }
}

const babelwires::ValueTreeNode& babelwires::MapEditor::getMapTreeNode(AccessModelScope& scope) const {
    const ValueTreeNode& mapTreeNode = ComplexValueEditor::getValueTreeNode(scope, getDataLocation());
    assert(mapTreeNode.getType().as<MapType>() || mapTreeNode.getType().as<SumOfMapsType>());
    return mapTreeNode;
}

const babelwires::MapValue& babelwires::MapEditor::getMapValueFromProject(AccessModelScope& scope) const {
    if (const ValueAssignmentData* const modifier = tryGetMapValueAssignmentData(scope)) {
        if (const MapValue* const mapValue = modifier->getValue()->as<MapValue>()) {
            return *mapValue;
        }
    }
    return getMapTreeNode(scope).getValue()->is<MapValue>();
}

const babelwires::ValueTreeNode* babelwires::MapEditor::tryGetMapTreeNode(AccessModelScope& scope) const {
    const ValueTreeNode* mapTreeNode = ComplexValueEditor::tryGetValueTreeNode(scope, getDataLocation());
    if (mapTreeNode->getType().as<MapType>() || mapTreeNode->getType().as<SumOfMapsType>()) {
        return mapTreeNode;
    }
    return nullptr;
}

const babelwires::ValueAssignmentData*
babelwires::MapEditor::tryGetMapValueAssignmentData(AccessModelScope& scope) const {
    const Node* const node = scope.getProject().getNode(getDataLocation().getNodeId());

    if (!node) {
        return nullptr;
    }

    const Modifier* const modifier = node->findModifier(getDataLocation().getPathToValue());

    if (!modifier) {
        return nullptr;
    }

    return modifier->getModifierData().as<ValueAssignmentData>();
}

babelwires::ValueHolderTemplate<babelwires::MapValue> babelwires::MapEditor::tryGetMapValueFromProject(AccessModelScope& scope) const {
    if (const ValueAssignmentData* const modifier = tryGetMapValueAssignmentData(scope)) {
        if (ValueHolderTemplate<MapValue> mapValue = modifier->getValue().asValueHolder<MapValue>()) {
            return mapValue;
        }
    }

    const babelwires::ValueTreeNode* const mapTreeNode = tryGetMapTreeNode(scope);
    if (!mapTreeNode) {
        return {};
    }

    return babelwires::ValueHolderTemplate<babelwires::MapValue>(mapTreeNode->getValue());
}

void babelwires::MapEditor::updateMapFromProject() {
    AccessModelScope scope(getProjectBridge());
    ValueHolderTemplate<MapValue> mapValueFromProject = tryGetMapValueFromProject(scope);
    if (mapValueFromProject) {
        getUserLogger().logInfo() << "Refreshing the map from the project";
        executeCommand(
            std::make_unique<SetMapCommand>("Refresh the map from the project", mapValueFromProject));
    } else {
        warnThatMapNoLongerInProject("Cannot refresh the map.");
    }
}

void babelwires::MapEditor::setEditorMap(const MapValue& map) {
    m_map.setMapValue(map);
    updateUiAfterChange();
}

void babelwires::MapEditor::updateUiAfterChange() const {
    m_mapModel->valuesChanged();
    if (m_map.getSourceTypeValidity()) {
        m_sourceTypeWidget->removeBadItemIfPresent();
    } else {
        m_sourceTypeWidget->addBadItemIfNotPresent(m_map.getCurrentSourceTypeRef());
    }
    if (m_map.getTargetTypeValidity()) {
        m_targetTypeWidget->removeBadItemIfPresent();
    } else {
        m_targetTypeWidget->addBadItemIfNotPresent(m_map.getCurrentTargetTypeRef());
    }
    m_sourceTypeWidget->setTypeRef(m_map.getCurrentSourceTypeRef());
    m_targetTypeWidget->setTypeRef(m_map.getCurrentTargetTypeRef());
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
            MapSerialization::saveToFile(filePathStr, m_map.extractMapValue());
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
                MapValue mapValue =
                    MapSerialization::loadFromFile(filePathStr, getProjectBridge().getContext(), getUserLogger());
                setEditorMap(mapValue);
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
    contents << getDataLocation().toString() << " - Map Editor";
    return contents.str().c_str();
}

void babelwires::MapEditor::warnThatMapNoLongerInProject(const std::string& operationDescription) {
    std::ostringstream contents;
    contents << "The map " << getDataLocation().toString() << " is no longer in the project.\n" << operationDescription;
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
    const TypeSystem& typeSystem = getProjectBridge().getContext().m_typeSystem;
    const MapType& mapType = m_typeRef.resolve(typeSystem).is<MapType>();
    ValueHolderTemplate<MapValue> defaultMapValue = mapType.createValue(typeSystem).m_valueHolder;
    executeCommand(std::make_unique<SetMapCommand>("Restore default map", std::move(defaultMapValue)));
}

void babelwires::MapEditor::setSourceTypeFromWidget() {
    const TypeRef& newSourceTypeRef = m_sourceTypeWidget->getTypeRef();
    if (newSourceTypeRef != m_map.getCurrentSourceTypeRef()) {
        executeCommand(std::make_unique<SetMapSourceTypeCommand>("Set map source type", newSourceTypeRef));
    }
}

void babelwires::MapEditor::setTargetTypeFromWidget() {
    const TypeRef& newTargetTypeRef = m_targetTypeWidget->getTypeRef();
    if (newTargetTypeRef != m_map.getCurrentTargetTypeRef()) {
        executeCommand(std::make_unique<SetMapTargetTypeCommand>("Set map target type", newTargetTypeRef));
    }
}
