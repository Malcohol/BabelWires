/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;
    class MapFeature;
    class MapModel;
    class MapView;
    class MapValueAssignmentData;

    class MapEditor : public ComplexValueEditor {
        public:
            /// data - Enough information to restore the state of a MapEditor.
            MapEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& m_logger, const ComplexValueEditorData& data);

            /// Resets the map editor to the state of the given map.
            void setEditorMap(const MapData& mapData);

            /// Apply the state of this map to the project.
            void applyMapToProject();

            /// Get the MapFeature or assert.
            const MapFeature& getMapFeature(AccessModelScope& scope) const;

            /// Get the mapData from the project or assert.
            const MapData& getMapDataFromProject(AccessModelScope& scope) const;

            /// Get the MapFeature or return nullptr.
            const MapFeature* tryGetMapFeature(AccessModelScope& scope) const;

            /// See if there is a map assignment.
            /// If there is, return its data.
            /// Otherwise return the data of the mapFeature.
            const MapData* tryGetMapDataFromProject(AccessModelScope& scope) const;

        protected:

            void updateMapFromProject();

            void saveMapToFile();
            bool trySaveMapToFile(const QString& filePath);
            void loadMapFromFile();

            /// Warn the user on apply or refresh that the map is no longer in the project.
            void warnThatMapNoLongerInProject(const std::string& operationDescription);

            /// Get a suitable title for the window carrying this widget.
            QString getTitle() const;

            /// Try to find a MapValueAssignment in the project which applies to the mapFeature.
            const MapValueAssignmentData* tryGetMapValueAssignmentData(AccessModelScope& scope) const;

        private:
            /// Manages the map data being edited.
            MapProject m_map;

            /// Manages changes to the map.
            CommandManager<MapProject> m_commandManager;

            QString m_lastSaveFilePath;

            MapView* m_mapView;
            MapModel* m_mapModel;
    };

}
