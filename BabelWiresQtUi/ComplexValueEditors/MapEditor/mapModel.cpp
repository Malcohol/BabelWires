/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

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

QVariant babelwires::MapModel::getMapEntryDisplayData(const DiscreteMapEntryData& entry, int column) const {
    if (column == 0) {
        return getMapEntryDisplayData(*entry.getSourceValue());
    } else {
        assert(column == 1);
        return getMapEntryDisplayData(*entry.getTargetValue());
    }
}

QVariant babelwires::MapModel::getMapEntryDisplayData(const AllToOneFallbackMapEntryData& entry, int column) const {
    if (column == 0) {
        return "*";
    } else {
        assert(column == 1);
        return getMapEntryDisplayData(*entry.getTargetValue());
    }
}

QVariant babelwires::MapModel::getMapEntryDisplayData(const IdentityFallbackMapEntryData& entry, int column) const {
    if (column == 0) {
        return "*";
    } else {
        assert(column == 1);
        return "= same";
    }
}

QVariant babelwires::MapModel::getMapEntryDisplayData(const Value& value) const {
    return "Value";
}


QVariant babelwires::MapModel::getMapEntryDisplayDataDispatcher(const MapEntryData& entry, int column) const {
    // There isn't an open-ended set of map entry types; I'm just using inheritance for convenience.
    if (const auto* discreteEntry = entry.as<DiscreteMapEntryData>()) {
        return getMapEntryDisplayData(*discreteEntry, column);
    } else if (const auto* allToOneEntry = entry.as<AllToOneFallbackMapEntryData>()) {
        return getMapEntryDisplayData(*allToOneEntry, column);
    } else if (const auto* identityEntry = entry.as<IdentityFallbackMapEntryData>()) {
        return getMapEntryDisplayData(*identityEntry, column);
    } else {
        assert(false && "Unknown map entry type");
    }
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const MapProjectEntry& entry = m_map.getMapEntry(index.row());
    switch (role) {
        case Qt::DisplayRole: {
            return getMapEntryDisplayDataDispatcher(entry.getData(), index.column());
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
