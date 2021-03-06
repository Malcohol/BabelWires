/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

#include <QtWidgets/QHeaderView>
#include <QMenu>

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

void babelwires::MapModel::initMapEntryModelDispatcher(const QModelIndex& index, MapEntryModelDispatcher& mapEntryModel) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const unsigned int column = static_cast<unsigned int>(index.column());
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry* entry = (row < mapProject.getNumMapEntries()) ? &mapProject.getMapEntry(row) : nullptr;
    const bool isLastRow = (row == mapProject.getNumMapEntries() - 1);
    mapEntryModel.init(*mapProject.getSourceType(), *mapProject.getTargetType(), entry, row, isLastRow);
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const MapEntryModel::Column column = MapEntryModel::indexToColumn(index);
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry& entry = mapProject.getMapEntry(row);

    MapEntryModelDispatcher mapEntryModel;
    initMapEntryModelDispatcher(index, mapEntryModel);

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
    initMapEntryModelDispatcher(index, mapEntryModel);

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
    initMapEntryModelDispatcher(index, mapEntryModel);

    const MapEntryModel::Column column = MapEntryModel::indexToColumn(index);
    const unsigned int row = static_cast<unsigned int>(index.row());
    if (m_mapEditor.getMapProject().getMapEntry(row).getValidity())
    {
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
