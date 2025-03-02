/**
 * QAction which opens an editor for a given value.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

namespace babelwires {

    struct OpenValueEditorAction : babelwires::NodeContentsContextMenuActionBase {
        OpenValueEditorAction(const QString& text, const DataLocation& location);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        std::unique_ptr<DataLocation> m_location;
    };

} // namespace babelwires
