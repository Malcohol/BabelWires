/**
 * QAction for the Change Source File action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>


namespace babelwires {

    /// QAction for the Change Source File action in the context menu.
    struct ChangeSourceFileAction : FeatureContextMenuAction {
        ChangeSourceFileAction();

        virtual void actionTriggered(FeatureModel& model, const QModelIndex& index) const override;
    };

} // namespace babelwires
