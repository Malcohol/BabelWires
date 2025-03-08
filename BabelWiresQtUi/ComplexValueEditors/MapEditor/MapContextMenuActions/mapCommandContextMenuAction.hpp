/**
 * This action adds an entry above the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <BabelWiresLib/Commands/commands.hpp>

#include <QAction>
#include <QAbstractItemModel>
#include <QModelIndex>

namespace babelwires {

    class MapProject;

    class MapCommandContextMenuAction : public ContextMenuAction {
        Q_OBJECT
      public:
        MapCommandContextMenuAction(std::unique_ptr<Command<MapProject>> command);

        void actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const override;

      private:
        std::unique_ptr<Command<MapProject>> m_command;
    };
} // namespace babelwires
