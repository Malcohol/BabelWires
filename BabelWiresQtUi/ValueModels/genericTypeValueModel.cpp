/**
 * Model for ArrayValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/genericTypeValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/setTypeVariableAction.hpp>

#include <BabelWiresLib/Types/Generic/genericValue.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

#include <Common/Log/debugLogger.hpp>

void babelwires::GenericTypeValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<ContextMenuEntry>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        auto entryGroup = std::make_unique<ContextMenuGroup>("Generic Type");
        if (const auto& projectDataLocation = location.as<ProjectDataLocation>()) {
            const GenericType& genericType = m_type->is<GenericType>();
            const GenericValue& genericValue = getValue()->is<GenericValue>();
            for (unsigned int i = 0; i < genericType.getNumVariables(); ++i) {
                auto setTypeVariable = std::make_unique<SetTypeVariableAction>(*projectDataLocation, i, true);
                // TODO tooltip.
                setTypeVariable->setEnabled(m_isStructureEditable);
                entryGroup->addContextMenuAction(std::move(setTypeVariable));
                auto resetTypeVariable = std::make_unique<SetTypeVariableAction>(*projectDataLocation, i, false);
                resetTypeVariable->setEnabled(m_isStructureEditable && (genericType.getTypeAssignment(getValue(), i) != TypeExp()));
                entryGroup->addContextMenuAction(std::move(resetTypeVariable));
            }
        } else {
            logDebug() << "Generic type encountered outside the project. No context menu options available.";
        }
        actionsOut.emplace_back(std::move(entryGroup));
    }
}
