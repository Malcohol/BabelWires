/**
 * Model for SumTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/sumValueModel.hpp>

#include <BabelWiresQtUi/ValueModels/valueModelDispatcher.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/mapCommandContextMenuAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/projectCommandContextMenuAction.hpp>

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>
#include <BabelWiresLib/Types/Map/Commands/resetMapValueCommand.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

void babelwires::SumValueModel::initializeValueModelDispatcherForSummand(ValueModelDispatcher& valueModel) const {
    const SumType& sumType = getType()->is<SumType>();
    const unsigned int currentIndex = sumType.getIndexOfValue(*m_typeSystem, *getValue());
    assert((currentIndex >= 0) && "Value not a valid instance of any summand of a SumType");
    const TypePtr& summandType = sumType.getSummands()[currentIndex];
    valueModel.init(*m_valueModelRegistry, *m_typeSystem, summandType, getValue(), m_isReadOnly, m_isStructureEditable);
}

bool babelwires::SumValueModel::isItemEditable() const {
    ValueModelDispatcher summandValueModel;
    initializeValueModelDispatcherForSummand(summandValueModel);
    return summandValueModel->isItemEditable();
}

QWidget* babelwires::SumValueModel::createEditor(QWidget* parent) const {
    ValueModelDispatcher summandValueModel;
    initializeValueModelDispatcherForSummand(summandValueModel);
    return summandValueModel->createEditor(parent);
}

void babelwires::SumValueModel::setEditorData(QWidget* editor) const {
    ValueModelDispatcher summandValueModel;
    initializeValueModelDispatcherForSummand(summandValueModel);
    return summandValueModel->setEditorData(editor);
}

babelwires::ValueHolder babelwires::SumValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    ValueModelDispatcher summandValueModel;
    initializeValueModelDispatcherForSummand(summandValueModel);
    return summandValueModel->createValueFromEditorIfDifferent(editor);
}

bool babelwires::SumValueModel::validateEditor(QWidget* editor) const {
    ValueModelDispatcher summandValueModel;
    initializeValueModelDispatcherForSummand(summandValueModel);
    return summandValueModel->validateEditor(editor);
}

void babelwires::SumValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<ContextMenuEntry>& entriesOut) const {
    ValueModel::getContextMenuActions(location, entriesOut);
    if (!m_isReadOnly) {
        auto group = std::make_unique<ContextMenuGroup>("SumType");
        const SumType& sumType = getType()->is<SumType>();
        const unsigned int currentIndex = sumType.getIndexOfValue(*m_typeSystem, *getValue());
        assert((currentIndex >= 0) && "Value not a valid instance of any summand of a SumType");
        const TypePtr& currentSummand = sumType.getSummands()[currentIndex];
        for (int i = 0; i < sumType.getSummands().size(); ++i) {
            const TypePtr& summandRef = sumType.getSummands()[i];
            std::unique_ptr<ContextMenuAction> action;
            // TODO Need some form of generalization here.
            if (const auto* mapLocation = location.as<MapProjectDataLocation>()) {
                action = std::make_unique<MapCommandContextMenuAction>(
                    std::make_unique<ResetMapValueCommand>(summandRef->getTypeExp().toString(), *mapLocation, summandRef->getTypeExp())
                );
                action->setCheckable(true);
            } else {
                // TODO This seems to work but is very awkward.
                const auto* projectLocation  = location.as<ProjectDataLocation>();
                assert(projectLocation);
                auto assignmentData = std::make_unique<ValueAssignmentData>(summandRef->createValue(*m_typeSystem));
                assignmentData->m_targetPath = projectLocation->getPathToValue();
                action = std::make_unique<ProjectCommandContextMenuAction>(
                    std::make_unique<AddModifierCommand>(summandRef->getTypeExp().toString(), projectLocation->getNodeId(), std::move(assignmentData))
                );
            }
            if (currentIndex == i) {
                action->setChecked(true);
                action->setEnabled(false);
            }
            group->addContextMenuAction(std::move(action));
        }
        entriesOut.emplace_back(std::move(group));
    }
}
