/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QTableView>

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

namespace babelwires {
    class MapEditor;
    class AllToOneFallbackMapEntryData;
    class AllToSameFallbackMapEntryData;
    class OneToOneMapEntryData;
    class MapEntryData;
    class Value;
    class Type;
    class MapEntryModelDispatcher;
    struct UiProjectContext;
    
    class MapView : public QTableView {
        Q_OBJECT
      public:
        MapView();
    };

    class MapModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        MapModel(QObject* parent, const UiProjectContext& projectContext, MapEditor& mapEditor);
        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        
        void getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut, const QModelIndex& index);

        MapEditor& getMapEditor() const;

      public slots:
        /// Trigger when the model has changed.
        void valuesChanged();

      public:
        /// Returns false if the index does not correspond to an entry in the map.
        bool initMapEntryModelDispatcher(const QModelIndex& index, MapEntryModelDispatcher& mapEntryModel) const;

      signals:
        void valuesMayHaveChanged() const;
        
      private:
        const UiProjectContext& m_projectContext;
        MapEditor& m_mapEditor;
    };
}
