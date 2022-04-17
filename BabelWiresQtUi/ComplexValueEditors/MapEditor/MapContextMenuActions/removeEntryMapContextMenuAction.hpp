/**
 * This action adds an entry above the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <QAction>
#include <QAbstractItemModel>
#include <QModelIndex>

namespace babelwires {

    /// Base class of context menu items in the context menu.
    class RemoveEntryMapContextMenuAction : public ContextMenuAction {
        Q_OBJECT
      public:
        RemoveEntryMapContextMenuAction(const QString& text, unsigned int indexOfEntry);

        void actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const override;

      private:
        unsigned int m_indexOfEntry;
    };
} // namespace babelwires