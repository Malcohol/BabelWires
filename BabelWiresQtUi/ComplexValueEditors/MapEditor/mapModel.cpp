/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>
#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/identityFallbackMapEntryData.hpp>
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

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const unsigned int column = static_cast<unsigned int>(index.column());
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry& entry = mapProject.getMapEntry(row);
    MapEntryModelDispatcher mapEntryModel;
    mapEntryModel.init(*mapProject.getSourceType(), *mapProject.getTargetType(), entry, row);

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

QMenu* babelwires::MapModel::getContextMenu(const QModelIndex& index) {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const unsigned int column = static_cast<unsigned int>(index.column());
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry& entry = mapProject.getMapEntry(row);
    MapEntryModelDispatcher mapEntryModel;
    mapEntryModel.init(*mapProject.getSourceType(), *mapProject.getTargetType(), entry, row);

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

babelwires::MapEditor& babelwires::MapModel::getMapEditor() {
    return m_mapEditor;
}

const babelwires::MapEditor& babelwires::MapModel::getMapEditor() const {
    return m_mapEditor;
}
