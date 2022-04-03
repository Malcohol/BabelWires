/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>

#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/identityFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>

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

babelwires::MapModel::MapModel(QObject* parent, MapProject& map)
    : QAbstractTableModel(parent)
    , m_map(map) {}

int babelwires::MapModel::rowCount(const QModelIndex& /*parent*/) const {
    return m_map.getNumMapEntries();
}

int babelwires::MapModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const MapProjectEntry& entry = m_map.getMapEntry(index.row());
    
    MapEntryModelDispatcher mapEntryModel;
    mapEntryModel.init(*m_map.getSourceType(), *m_map.getTargetType(), entry.getData());

    unsigned int column = static_cast<unsigned int>(index.column());

    switch (role) {
        case Qt::DisplayRole: {
            return mapEntryModel->getDisplayData(column);
        }
        case Qt::ToolTipRole: {
            const std::string& reasonForFailure = entry.getReasonForFailure();
            if (!reasonForFailure.empty()) {
                return QString(reasonForFailure.c_str());
            } else {
                return {};
            }
        }
        case Qt::BackgroundRole: {
            if (entry.getReasonForFailure().empty()) {
                return QBrush(QColor(255, 255, 255));
            } else {
                return QBrush(QColor(255, 0, 0));
            }
        }
    }
    return {};
}
