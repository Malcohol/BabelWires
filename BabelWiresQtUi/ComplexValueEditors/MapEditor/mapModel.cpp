/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresLib/Maps/map.hpp>

#include <QtWidgets/QHeaderView>

babelwires::MapView::MapView() {
    // setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    verticalHeader()->setVisible(false);
    //verticalHeader()->setStretchLastSection(true);
    //verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

babelwires::MapModel::MapModel(QObject* parent, Map& map)
    : QAbstractTableModel(parent)
    , m_map(map) {}

int babelwires::MapModel::rowCount(const QModelIndex& /*parent*/) const {
    return 25;
    //return m_map.getNumMapEntries();
}

int babelwires::MapModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    return {};
}
