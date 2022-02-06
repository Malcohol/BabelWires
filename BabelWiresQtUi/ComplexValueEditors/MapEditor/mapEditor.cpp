/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

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

        {
            QLabel* label = new QLabel(topButtons);
            label->setText("Source type: ");
            topButtonsLayout->addWidget(label);
        }
        TypeWidget* sourceTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedSourceIds());
        topButtonsLayout->addWidget(sourceTypes);

        {
            QLabel* label = new QLabel(topButtons);
            label->setText("Target type: ");
            topButtonsLayout->addWidget(label);
        }
        TypeWidget* targetTypes = new TypeWidget(topButtons, projectBridge, mapFeature.getAllowedTargetIds());
        topButtonsLayout->addWidget(targetTypes);

        show();
    }

void babelwires::MapEditor::applyMapToProject() {
    auto modifierData = std::make_unique<MapValueAssignmentData>();
    modifierData->m_map = m_map;
    modifierData->m_pathToFeature = getData().getPathToValue();

    auto setValueCommand = std::make_unique<AddModifierCommand>("Set map value", getData().getElementId(), std::move(modifierData));
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
