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
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>

#include <BabelWiresLib/Project/projectIds.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectGraphModel;
    class MapModel;
    class MapView;
    struct ValueAssignmentData;
    class TypeWidget;
    class ProjectDataLocation;

    class MapEditor : public ComplexValueEditor {
      Q_OBJECT
      public:
        /// data - Enough information to restore the state of a MapEditor.
        MapEditor(QWidget* parent, ProjectGraphModel& projectGraphModel, UserLogger& userLogger,
                  const ProjectDataLocation& data);

        /// Resets the map editor to the state of the given map.
        void setEditorMap(const MapValue& mapValue);

        /// Apply the state of this map to the project.
        void applyMapToProject();

        /// Get the ValueTreeNode holding the map or assert.
        const ValueTreeNode& getMapTreeNode(const AccessModelScope& scope) const;

        /// Get the MapValue from the project or assert.
        const MapValue& getMapValueFromProject(const AccessModelScope& scope) const;

        /// Get the ValueTreeNode holding the map or return nullptr.
        const ValueTreeNode* tryGetMapTreeNode(const AccessModelScope& scope) const;

        /// See if there is a map assignment.
        /// If there is, return its data.
        /// Otherwise return the data of the ValueTreeNode.
        ValueHolder tryGetMapValueFromProject(const AccessModelScope& scope) const;

        const MapProject& getMapProject() const;

        void executeCommand(std::unique_ptr<Command<MapProject>> command);

        void setToDefault();

      protected:
        bool maybeApplyToProject() override;
        
        void updateMapFromProject();

        void saveMapToFile();
        bool trySaveMapToFile(const QString& filePath);
        void loadMapFromFile();

        /// Warn the user on apply or refresh that the map is no longer in the project.
        void warnThatMapNoLongerInProject(const std::string& operationDescription);

        /// Get a suitable title for the window carrying this widget.
        QString getTitle() const;

        /// Try to find a MapValueAssignment in the project which applies to the Map.
        const ValueAssignmentData* tryGetMapValueAssignmentData(const AccessModelScope& scope) const;

        void updateUiAfterChange() const;

        void updateTypeOptions() const;

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

        /// Stores the default type.
        TypeExp m_typeExp;

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
