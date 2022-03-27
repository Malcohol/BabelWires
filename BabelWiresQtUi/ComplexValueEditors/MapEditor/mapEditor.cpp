/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Maps/mapSerialization.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>

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
        auto contentsButtons = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults);
        {
            QPushButton* saveButton = new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save copy to file");
            contentsButtons->addButton(saveButton, QDialogButtonBox::ButtonRole::AcceptRole);
            connect(saveButton, &QAbstractButton::clicked, this, &MapEditor::saveMapToFile);
        }
        {
            QPushButton* loadButton = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "Load from file");
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
            const MapFeature& mapFeature = getMapFeature(scope);
            const MapData& mapData = getMapDataFromProject(scope);
            setEditorMap(mapData);
            {
                typeBarLayout->addWidget(new QLabel("Source type: ", typeBar));
                TypeWidget* sourceTypes = new TypeWidget(typeBar, projectBridge, mapFeature.getAllowedSourceIds());
                typeBarLayout->addWidget(sourceTypes);
            }
            {
                typeBarLayout->addWidget(new QLabel("Target type: ", typeBar));
                TypeWidget* targetTypes = new TypeWidget(typeBar, projectBridge, mapFeature.getAllowedTargetIds());
                typeBarLayout->addWidget(targetTypes);
            }
            // TODO Connect type widgets.
        }
        m_mapView = new MapView;
        m_mapModel = new MapModel(m_mapView, m_map);
        m_mapView->setModel(m_mapModel);
        contentsLayout->addWidget(m_mapView);
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
    }
}

const babelwires::MapFeature& babelwires::MapEditor::getMapFeature(AccessModelScope& scope) const {
    const ValueFeature& valueFeature = ComplexValueEditor::getValueFeature(scope, getData());
    const MapFeature* mapFeature = valueFeature.as<MapFeature>();
    assert(mapFeature && "The value feature was not a map feature");
    return *mapFeature;
}

const babelwires::MapData& babelwires::MapEditor::getMapDataFromProject(AccessModelScope& scope) const {
    const MapValueAssignmentData *const mapModifier = tryGetMapValueAssignmentData(scope);
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

const babelwires::MapValueAssignmentData* babelwires::MapEditor::tryGetMapValueAssignmentData(AccessModelScope& scope) const {
    const FeatureElement *const element = scope.getProject().getFeatureElement(getData().getElementId());

    if (!element) {
        return nullptr;
    }

    const Modifier *const modifier = element->findModifier(getData().getPathToValue());

    if (!modifier) {
        return nullptr;
    }

    return modifier->getModifierData().as<MapValueAssignmentData>();
}

const babelwires::MapData* babelwires::MapEditor::tryGetMapDataFromProject(AccessModelScope& scope) const {
    const MapValueAssignmentData *const mapModifier = tryGetMapValueAssignmentData(scope);

    if (mapModifier) {
        return &mapModifier->m_mapData;
    }

    const babelwires::MapFeature *const mapFeature = tryGetMapFeature(scope);
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
        setEditorMap(*mapDataFromProject);
    } else {
        warnThatMapNoLongerInProject("Cannot refresh the map.");
    }
}

void babelwires::MapEditor::setEditorMap(const MapData& map) {
    m_map.setMapData(map);
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
                MapData mapData = MapSerialization::loadFromFile(filePathStr, getProjectBridge().getContext(), getUserLogger());
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