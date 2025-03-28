/**
 * QAction for the save file as action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

namespace babelwires {

    struct SaveFileAsAction : babelwires::NodeContentsContextMenuActionBase {
        SaveFileAsAction();

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;
    };

} // namespace babelwires
