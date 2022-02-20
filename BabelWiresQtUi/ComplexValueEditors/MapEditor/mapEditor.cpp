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

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Maps/mapSerialization.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

babelwires::MapEditor::MapEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                                 const ComplexValueEditorData& data)
    : ComplexValueEditor(parent, projectBridge, userLogger, data)
    , m_commandManager(m_map, userLogger) {
    QLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    QWidget* topButtons = new QWidget(this);
    mainLayout->addWidget(topButtons);

    QLayout* topButtonsLayout = new QHBoxLayout();
    topButtons->setLayout(topButtonsLayout);

    AccessModelScope scope(getProjectBridge());
    const MapFeature& mapFeature = getMapFeature(scope);

    topButtonsLayout->addWidget(new QLabel("Source type: ", topButtons));
    TypeWidget* sourceTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedSourceIds());
    topButtonsLayout->addWidget(sourceTypes);

    topButtonsLayout->addWidget(new QLabel("Target type: ", topButtons));
    TypeWidget* targetTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedTargetIds());
    topButtonsLayout->addWidget(targetTypes);

    auto contentsButtons =
        new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Save | QDialogButtonBox::Open);

    mainLayout->addWidget(contentsButtons);

    auto bottomButtons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Close);
    connect(bottomButtons->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this,
            &MapEditor::applyMapToProject);
    connect(bottomButtons, &QDialogButtonBox::rejected, this, &MapEditor::close);
    connect(contentsButtons->button(QDialogButtonBox::Save), &QAbstractButton::clicked, this,
            &MapEditor::saveMap);

    mainLayout->addWidget(bottomButtons);

    show();
}

void babelwires::MapEditor::applyMapToProject() {
    auto modifierData = std::make_unique<MapValueAssignmentData>();
    modifierData->m_map = m_map;
    modifierData->m_pathToFeature = getData().getPathToValue();

    auto setValueCommand =
        std::make_unique<AddModifierCommand>("Set map value", getData().getElementId(), std::move(modifierData));
    getProjectBridge().scheduleCommand(std::move(setValueCommand));
}

const babelwires::MapFeature& babelwires::MapEditor::getMapFeature(AccessModelScope& scope) {
    const ValueFeature& valueFeature = ComplexValueEditor::getValueFeature(scope, getData());
    const MapFeature* mapFeature = valueFeature.as<MapFeature>();
    assert(mapFeature && "The value feature was not map feature");
    return *mapFeature;
}

void babelwires::MapEditor::updateMapFromProject() {
    AccessModelScope scope(getProjectBridge());
    const MapFeature& mapFeature = getMapFeature(scope);
    setEditorMap(mapFeature.get());
}

void babelwires::MapEditor::setEditorMap(const Map& map) {
    m_map = map;
}

void babelwires::MapEditor::saveMap() 
{
    QString dialogCaption = tr("Save project as");
    QString dialogFormats = tr("Map (*.bw_map)");
    QString filePath = QFileDialog::getSaveFileName(this, dialogCaption,
                                                    m_lastSaveFilePath, dialogFormats);
    if (!filePath.isNull()) {
        const QString ext = ".bw_map";
        if (!filePath.endsWith(ext)) {
            filePath += ext;
        }
        trySaveMap(filePath);
        m_lastSaveFilePath = filePath;
    }
}

bool babelwires::MapEditor::trySaveMap(const QString& filePath) {
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
