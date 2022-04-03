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
    class MapProject;
    class AllToOneFallbackMapEntryData;
    class IdentityFallbackMapEntryData;
    class DiscreteMapEntryData;
    class MapEntryData;
    class Value;
    class Type;
    
    class MapView : public QTableView {
        Q_OBJECT
      public:
        MapView();
    };

    class MapModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        MapModel(QObject* parent, MapProject& map);
        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        //Qt::ItemFlags flags(const QModelIndex& index) const override;
        
        /// May return null or a new menu.
        QMenu* getContextMenu(const QModelIndex& index);

        MapProject& getMapProject();
        const MapProject& getMapProject() const;

      private:
        MapProject& m_map;
    };
}
