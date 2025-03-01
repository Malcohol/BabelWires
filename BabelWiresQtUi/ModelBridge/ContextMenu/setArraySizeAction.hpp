/**
 * QAction which opens the set array size dialog
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

namespace babelwires {

    struct SetArraySizeAction : babelwires::NodeContentsContextMenuActionBase {
        SetArraySizeAction(babelwires::Path pathToArray);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        babelwires::Path m_pathToArray;
    };

} // namespace babelwires
