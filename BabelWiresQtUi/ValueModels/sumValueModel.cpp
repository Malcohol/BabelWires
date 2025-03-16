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

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>
#include <BabelWiresLib/Types/Map/Commands/resetMapValueCommand.hpp>

void babelwires::SumValueModel::initializeValueModelDispatcherForSummand(ValueModelDispatcher& valueModel) const {
    const SumType& sumType = getType()->is<SumType>();
    const unsigned int currentIndex = sumType.getIndexOfValue(*m_typeSystem, *getValue());
    assert((currentIndex >= 0) && "Value not a valid instance of any summand of a SumType");
    const TypeRef& summandRef = sumType.getSummands()[currentIndex];
    const Type& summandType = summandRef.resolve(*m_typeSystem);
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

babelwires::EditableValueHolder babelwires::SumValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
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
        const TypeRef& currentSummandRef = sumType.getSummands()[currentIndex];
        for (int i = 0; i < sumType.getSummands().size(); ++i) {
            const TypeRef& summandRef = sumType.getSummands()[i];
            std::unique_ptr<ContextMenuAction> action;
            // TODO Need some form of generalization here.
            if (const auto* mapLocation = location.as<MapProjectDataLocation>()) {
                action = std::make_unique<MapCommandContextMenuAction>(
                    std::make_unique<ResetMapValueCommand>(summandRef.toString(), *mapLocation, summandRef)
                );
                action->setCheckable(true);
            } else {
                // TODO Add support for sum types in the project.
                assert(false && "Unimplemented");
                //auto selectSummand = std::make_unique<SelectSummandCommand>(location.getNodeId(), location.getPathToValue(), i);
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
