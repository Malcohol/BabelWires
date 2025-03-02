/**
 * This action changes the kind of entry at the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

#include <QAction>
#include <QAbstractItemModel>
#include <QModelIndex>

namespace babelwires {

    class ChangeEntryKindContextMenuAction : public ContextMenuAction {
        Q_OBJECT
      public:
        ChangeEntryKindContextMenuAction(const QString& text, MapEntryData::Kind kind, unsigned int indexOfEntry, bool isCheckable);

        void actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const override;

      private:
        MapEntryData::Kind m_kind;
        unsigned int m_indexOfNewEntry;
    };
} // namespace babelwires
