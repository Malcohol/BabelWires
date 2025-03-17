/**
 * Model for MapValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/mapValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp>

#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <Common/Log/debugLogger.hpp>

void babelwires::MapValueModel::getContextMenuActions(const DataLocation& location,
                                                      std::vector<ContextMenuEntry>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        if (const auto& projectDataLocation = location.as<ProjectDataLocation>()) {
            auto editAction = std::make_unique<OpenValueEditorAction>("Open map editor", *projectDataLocation);
            actionsOut.emplace_back(std::move(editAction));
        } else {
            logDebug() << "Record with optionals encountered outside the project. No context menu options available.";
        }
    }
}
