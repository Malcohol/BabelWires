

#include <BabelWiresLib/Maps/map.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;

    class MapEditor : public QWidget {
        public:
            /// elementWithMap - the element in the project carrying the map we're editing.
            /// pathToMap - the path within the element at which the map exists.
            MapEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& m_logger, ElementId elementWithMap, FeaturePath pathToMap);

            /// Resets the map editor to the state of the given map.
            void setEditorMap(const Map& map);

            /// Apply the state of this map to the project.
            void applyMapToProject();

        private:
            /// Needed when making changes to the project.
            ProjectBridge& m_projectBridge;

            /// The element in the project carrying the map we're editing.
            ElementId m_elementWithMap;

            /// The path within the element at which the map exists.
            FeaturePath m_pathToMap;

            /// A local copy of the map being edited.
            Map m_map;

            /// Manages changes to the map.
            CommandManager<Map> m_commandManager;
    };

}
