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

      private:
        QVariant getMapEntryDisplayDataDispatcher(const MapEntryData& entry, int column) const;
        QVariant getMapEntryDisplayData(const DiscreteMapEntryData& entry, int column) const;
        QVariant getMapEntryDisplayData(const AllToOneFallbackMapEntryData& entry, int column) const;
        QVariant getMapEntryDisplayData(const IdentityFallbackMapEntryData& entry, int column) const;
        QVariant getMapEntryDisplayData(const Value& value) const;

      private:
        MapProject& m_map;
    };
}
