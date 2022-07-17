/**
 * QAction for the reload file action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

namespace babelwires {

    /// QAction for the reload file action in the context menu.
    struct ReloadFileAction : babelwires::FeatureContextMenuAction {
        ReloadFileAction();

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;
    };

} // namespace babelwires