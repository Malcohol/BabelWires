/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

#include <QMenu>
#include <QtWidgets/QHeaderView>

babelwires::MapView::MapView() {
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    verticalHeader()->setVisible(false);
    // verticalHeader()->setStretchLastSection(true);
    // verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

babelwires::MapModel::MapModel(QObject* parent, MapEditor& mapEditor)
    : QAbstractTableModel(parent)
    , m_mapEditor(mapEditor) {}

int babelwires::MapModel::rowCount(const QModelIndex& /*parent*/) const {
    return m_mapEditor.getMapProject().getNumMapEntries();
}

int babelwires::MapModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

bool babelwires::MapModel::initMapEntryModelDispatcher(const QModelIndex& index,
                                                       MapEntryModelDispatcher& mapEntryModel) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const unsigned int column = static_cast<unsigned int>(index.column());
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const unsigned int numMapEntries = mapProject.getNumMapEntries();
    if ((row >= numMapEntries) || (column >= 2)) {
        // This can happen if you right-click below the current set of entries.
        return false;
    }
    const MapProjectEntry& entry = mapProject.getMapEntry(row);
    const bool isLastRow = (row == numMapEntries - 1);
    mapEntryModel.init(*mapProject.getSourceType(), *mapProject.getTargetType(), entry, row, isLastRow);
    return true;
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const MapEntryModel::Column column = MapEntryModel::indexToColumn(index);
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry& entry = mapProject.getMapEntry(row);

    MapEntryModelDispatcher mapEntryModel;
    if (!initMapEntryModelDispatcher(index, mapEntryModel)) {
        return {};
    }

    switch (role) {
        case Qt::DisplayRole: {
            return mapEntryModel->getDisplayData(column);
        }
        case Qt::ToolTipRole: {
            const Result validity = entry.getValidity();
            if (!validity) {
                return QString(validity.getReasonWhyFailed().c_str());
            } else {
                return {};
            }
        }
        case Qt::BackgroundRole: {
            const Result validity = entry.getValidity();
            if (!validity) {
                return QBrush(QColor(255, 0, 0));
            } else {
                return QBrush(QColor(255, 255, 255));
            }
        }
    }
    return {};
}

QMenu* babelwires::MapModel::getContextMenu(const QModelIndex& index) {
    MapEntryModelDispatcher mapEntryModel;
    if (!initMapEntryModelDispatcher(index, mapEntryModel)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ContextMenuAction>> actions;
    mapEntryModel->getContextMenuActions(actions);
    if (!actions.empty()) {
        ContextMenu* menu = new ContextMenu(*this, index);
        for (auto&& action : actions) {
            menu->addContextMenuAction(action.release());
        }
        return menu;
    }
    return nullptr;
}

babelwires::MapEditor& babelwires::MapModel::getMapEditor() const {
    return m_mapEditor;
}

Qt::ItemFlags babelwires::MapModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    MapEntryModelDispatcher mapEntryModel;
    if (!initMapEntryModelDispatcher(index, mapEntryModel)) {
        return flags;
    }

    const MapEntryModel::Column column = MapEntryModel::indexToColumn(index);
    const unsigned int row = static_cast<unsigned int>(index.row());
    if (m_mapEditor.getMapProject().getMapEntry(row).getValidity()) {
        if (mapEntryModel->isItemEditable(column)) {
            flags = flags | Qt::ItemIsEditable;
        }
    }
    return flags;
}

void babelwires::MapModel::valuesChanged() {
    layoutChanged();
    emit valuesMayHaveChanged();
}
