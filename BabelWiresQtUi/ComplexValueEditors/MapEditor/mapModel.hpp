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
    class Map;

    class MapView : public QTableView {
        Q_OBJECT
      public:
        MapView();
    };

    class MapModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        MapModel(QObject* parent, Map& map);
        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        //Qt::ItemFlags flags(const QModelIndex& index) const override;
      private:
        Map& m_map;
    };
}
