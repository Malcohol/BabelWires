/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWiresLib/Features/Path/featurePath.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"

namespace babelwires {

    /// QAction for the insert entry into array action in the context menu.
    class InsertArrayEntryAction : public babelwires::FeatureContextMenuAction {
      public:
        InsertArrayEntryAction(babelwires::FeaturePath pathToArray, int indexOfNewEntry);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::FeaturePath m_pathToArray;
        int m_indexOfNewEntry;
    };

} // namespace babelwires
