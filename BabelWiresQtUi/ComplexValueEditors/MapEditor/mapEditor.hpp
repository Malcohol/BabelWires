/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;
    class MapFeature;
    class MapModel;
    class MapView;
    class MapValueAssignmentData;
    class TypeWidget;

    class MapEditor : public ComplexValueEditor {
      Q_OBJECT
      public:
        /// data - Enough information to restore the state of a MapEditor.
        MapEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                  const ComplexValueEditorData& data);

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

        const MapProject& getMapProject() const;

        void executeCommand(std::unique_ptr<Command<MapProject>> command);

        void setToDefault();

      protected:
        bool maybeApply() override;
        
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

      public slots:
        void onCustomContextMenuRequested(const QPoint& pos);

      private slots:
        void undo();
        void redo();
        void setSourceTypeFromWidget();
        void setTargetTypeFromWidget();

      private:
        void onUndoStateChanged();

      private:
        /// Manages the map data being edited.
        MapProject m_map;

        /// Manages changes to the map.
        CommandManager<MapProject> m_commandManager;

        QString m_lastSaveFilePath;

        std::unique_ptr<QAction> m_undoAction;
        std::unique_ptr<QAction> m_redoAction;
        SignalSubscription m_undoStateChangedSubscription;

        MapView* m_mapView;
        MapModel* m_mapModel;

        TypeWidget* m_sourceTypeWidget;
        TypeWidget* m_targetTypeWidget;
    };

} // namespace babelwires
