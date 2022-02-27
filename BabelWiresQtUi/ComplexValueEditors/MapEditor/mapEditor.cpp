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

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Maps/mapSerialization.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#define MAP_FILE_EXTENSION ".bw_map"
#define MAP_FORMAT_STRING "Map (*" MAP_FILE_EXTENSION ")"

babelwires::MapEditor::MapEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                                 const ComplexValueEditorData& data)
    : ComplexValueEditor(parent, projectBridge, userLogger, data)
    , m_commandManager(m_map, userLogger) {
    setWindowTitle(getTitle());
    QLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    QWidget* topButtons = new QWidget(this);
    mainLayout->addWidget(topButtons);

    QLayout* topButtonsLayout = new QHBoxLayout();
    topButtons->setLayout(topButtonsLayout);

    AccessModelScope scope(getProjectBridge());
    const MapFeature& mapFeature = getMapFeature(scope);
    setEditorMap(mapFeature.get());

    topButtonsLayout->addWidget(new QLabel("Source type: ", topButtons));
    TypeWidget* sourceTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedSourceIds());
    topButtonsLayout->addWidget(sourceTypes);

    topButtonsLayout->addWidget(new QLabel("Target type: ", topButtons));
    TypeWidget* targetTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedTargetIds());
    topButtonsLayout->addWidget(targetTypes);

    auto contentsButtons =
        new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Save | QDialogButtonBox::Open);

    mainLayout->addWidget(contentsButtons);
    auto bottomButtons =
        new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton* reloadButton = new QPushButton(style()->standardIcon(QStyle::SP_BrowserReload), "Refresh from project");
    QPushButton* applyButton = new QPushButton(style()->standardIcon(QStyle::SP_DialogOkButton), "Apply to project");
    bottomButtons->addButton(reloadButton, QDialogButtonBox::ButtonRole::ResetRole);
    bottomButtons->addButton(applyButton, QDialogButtonBox::ButtonRole::ApplyRole);
    mainLayout->addWidget(bottomButtons);

    connect(contentsButtons->button(QDialogButtonBox::Save), &QAbstractButton::clicked, this,
            &MapEditor::saveMapToFile);
    connect(contentsButtons->button(QDialogButtonBox::Open), &QAbstractButton::clicked, this,
            &MapEditor::loadMapFromFile);

    connect(applyButton, &QAbstractButton::clicked, this,
            &MapEditor::applyMapToProject);
    connect(reloadButton, &QAbstractButton::clicked, this,
            &MapEditor::updateMapFromProject);
    connect(bottomButtons, &QDialogButtonBox::rejected, this, &MapEditor::close);

    show();
}

void babelwires::MapEditor::applyMapToProject() {
    auto modifierData = std::make_unique<MapValueAssignmentData>();
    modifierData->m_map = m_map;
    modifierData->m_pathToFeature = getData().getPathToValue();

    auto setValueCommand =
        std::make_unique<AddModifierCommand>("Set map value", getData().getElementId(), std::move(modifierData));
    if (!getProjectBridge().executeCommandSynchronously(std::move(setValueCommand))) {
        warnThatMapNoLongerInProject("Cannot apply the map.");
    }
}

const babelwires::MapFeature& babelwires::MapEditor::getMapFeature(AccessModelScope& scope) {
    const ValueFeature& valueFeature = ComplexValueEditor::getValueFeature(scope, getData());
    const MapFeature* mapFeature = valueFeature.as<MapFeature>();
    assert(mapFeature && "The value feature was not a map feature");
    return *mapFeature;
}

const babelwires::MapFeature* babelwires::MapEditor::tryGetMapFeature(AccessModelScope& scope) {
    const ValueFeature* valueFeature = ComplexValueEditor::tryGetValueFeature(scope, getData());
    if (valueFeature) {
        const MapFeature* mapFeature = valueFeature->as<MapFeature>();
        if (mapFeature) {
            return mapFeature;
        }
    }
    return nullptr;
}

void babelwires::MapEditor::updateMapFromProject() {
    AccessModelScope scope(getProjectBridge());
    const MapFeature* mapFeature = tryGetMapFeature(scope);
    if (mapFeature) {
        getUserLogger().logInfo() << "Refreshing the map from the project";
        setEditorMap(mapFeature->get());
    } else {
        warnThatMapNoLongerInProject("Cannot refresh the map.");
    }
}

void babelwires::MapEditor::setEditorMap(const Map& map) {
    m_map = map;
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
            MapSerialization::saveToFile(filePathStr, m_map);
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
                Map map = MapSerialization::loadFromFile(filePathStr, getProjectBridge().getContext(), getUserLogger());
                m_map = map;
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