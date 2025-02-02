/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

namespace babelwires {

    /// QAction for the insert entry into array action in the context menu.
    class InsertArrayEntryAction : public babelwires::FeatureContextMenuAction {
      public:
        InsertArrayEntryAction(const QString& text, babelwires::Path pathToArray, int indexOfNewEntry);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        babelwires::Path m_pathToArray;
        int m_indexOfNewEntry;
    };

} // namespace babelwires
