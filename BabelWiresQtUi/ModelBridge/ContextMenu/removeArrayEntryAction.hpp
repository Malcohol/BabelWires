/**
 * QAction for the remove entry from array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

namespace babelwires {

    /// QAction for the remove entry from array action in the context menu.
    class RemoveArrayEntryAction : public babelwires::FeatureContextMenuAction {
      public:
        RemoveArrayEntryAction(babelwires::FeaturePath pathToArray, int indexOfEntryToRemove);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::FeaturePath m_pathToArray;
        int m_indexOfEntryToRemove;
    };

} // namespace babelwires
