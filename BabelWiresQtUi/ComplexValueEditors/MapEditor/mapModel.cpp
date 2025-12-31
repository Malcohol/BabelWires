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
#include <BabelWiresQtUi/uiProjectContext.hpp>
#include <BabelWiresQtUi/Utilities/colours.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

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

babelwires::MapModel::MapModel(QObject* parent, const UiProjectContext& projectContext, MapEditor& mapEditor)
    : QAbstractTableModel(parent)
    , m_projectContext(projectContext)
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
    mapEntryModel.init(m_projectContext.m_valueModelReg, m_projectContext.m_typeSystem, mapProject.getCurrentSourceType(), mapProject.getCurrentTargetType(), entry, row, static_cast<MapEntryModel::Column>(column), isLastRow);
    return true;
}

QVariant babelwires::MapModel::data(const QModelIndex& index, int role) const {
    const unsigned int row = static_cast<unsigned int>(index.row());
    const MapProject& mapProject = m_mapEditor.getMapProject();
    const MapProjectEntry& entry = mapProject.getMapEntry(row);

    MapEntryModelDispatcher mapEntryModel;
    if (!initMapEntryModelDispatcher(index, mapEntryModel)) {
        return {};
    }

    switch (role) {
        case Qt::DisplayRole: {
            return mapEntryModel->getDisplayData();
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
            return getBackgroundColour(entry.getValidity() ? BackgroundType::normal : BackgroundType::failure);
        }
    }
    return {};
}

void babelwires::MapModel::getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut, const QModelIndex& index) {
    MapEntryModelDispatcher mapEntryModel;
    if (!initMapEntryModelDispatcher(index, mapEntryModel)) {
        return;
    }

    mapEntryModel->getContextMenuActions(actionsOut);
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

    const unsigned int row = static_cast<unsigned int>(index.row());
    if (m_mapEditor.getMapProject().getMapEntry(row).getValidity()) {
        if (mapEntryModel->isItemEditable()) {
            flags = flags | Qt::ItemIsEditable;
        }
    }
    return flags;
}

void babelwires::MapModel::valuesChanged() {
    layoutChanged();
    emit valuesMayHaveChanged();
}
