/**
 * QAction for the remove entry from array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

namespace babelwires {

    /// QAction for the remove entry from array action in the context menu.
    class RemoveArrayEntryAction : public babelwires::NodeContentsContextMenuActionBase {
      public:
        RemoveArrayEntryAction(babelwires::Path pathToArray, int indexOfEntryToRemove);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        babelwires::Path m_pathToArray;
        int m_indexOfEntryToRemove;
    };

} // namespace babelwires
