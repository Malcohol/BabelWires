/**
 * QAction for the remove failed modifiers action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

namespace babelwires {

    /// QAction for the remove failed modifiers action in the context menu.
    struct RemoveFailedModifiersAction : NodeContentsContextMenuActionBase {
        RemoveFailedModifiersAction();

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;
    };

} // namespace babelwires
