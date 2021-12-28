
#include <BabelWiresQtUi/MapEditor/mapEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWires/BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp>
#include <BabelWires/BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

babelwires::MapEditor::MapEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                                 ElementId elementWithMap, FeaturePath pathToMap)
    : QWidget(parent)
    , m_projectBridge(projectBridge)
    , m_elementWithMap(elementWithMap)
    , m_pathToMap(pathToMap)
    , m_commandManager(m_map, userLogger) {
        // TODO Start adding widgets
        QLayout* mainLayout = new QVBoxLayout();
        setLayout(mainLayout);

        QWidget* topButtons = new QWidget(this);
        mainLayout->addWidget(topButtons);

        QLayout* topButtonsLayout = new QHBoxLayout();
        topButtons->setLayout(topButtonsLayout);

        {
            QLabel* label = new QLabel(topButtons);
            label->setText("Flerm");
            topButtonsLayout->addWidget(label);
        }
        {
            QLabel* label = new QLabel(topButtons);
            label->setText("Erg");
            topButtonsLayout->addWidget(label);
        }
        show();
    }

void babelwires::MapEditor::applyMapToProject() {
    auto modifierData = std::make_unique<MapValueAssignmentData>();
    modifierData->m_map = m_map;
    modifierData->m_pathToFeature = m_pathToMap;

    auto setValueCommand = std::make_unique<AddModifierCommand>("Set map value", m_elementWithMap, std::move(modifierData));
    m_projectBridge.scheduleCommand(std::move(setValueCommand));
}

void babelwires::MapEditor::setEditorMap(const Map& map) {
    // TODO Reset command manager? Add a command for this?
    m_map = map;
}

