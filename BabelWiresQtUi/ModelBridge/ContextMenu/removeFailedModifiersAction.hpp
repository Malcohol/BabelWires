/**
 * QAction for the remove failed modifiers action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"

namespace babelwires {

    /// QAction for the remove failed modifiers action in the context menu.
    struct RemoveFailedModifiersAction : FeatureContextMenuAction {
        RemoveFailedModifiersAction();

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;
    };

} // namespace babelwires
