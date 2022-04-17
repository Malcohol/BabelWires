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

namespace babelwires {
    class MapEditor;
    class AllToOneFallbackMapEntryData;
    class IdentityFallbackMapEntryData;
    class DiscreteMapEntryData;
    class MapEntryData;
    class Value;
    class Type;
    class MapEntryModelDispatcher;
    
    class MapView : public QTableView {
        Q_OBJECT
      public:
        MapView();
    };

    class MapModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        MapModel(QObject* parent, MapEditor& mapEditor);
        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        //Qt::ItemFlags flags(const QModelIndex& index) const override;
        
        /// May return null or a new menu.
        QMenu* getContextMenu(const QModelIndex& index);

        MapEditor& getMapEditor();
        const MapEditor& getMapEditor() const;

      public:
        void initMapEntryModelDispatcher(const QModelIndex& index, MapEntryModelDispatcher& mapEntryModel) const;

      private:
        MapEditor& m_mapEditor;
    };
}
