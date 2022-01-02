/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditor.hpp>

#include <BabelWiresLib/Maps/map.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;

    class MapEditor : public ValueEditor {
        public:
            /// data - Enough information to restore the state of a MapEditor.
            MapEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& m_logger, const ValueEditorData& data);

            /// Resets the map editor to the state of the given map.
            void setEditorMap(const Map& map);

            /// Apply the state of this map to the project.
            void applyMapToProject();

        private:
            /// A local copy of the map being edited.
            Map m_map;

            /// Manages changes to the map.
            CommandManager<Map> m_commandManager;
    };

}
