/**
 * QAction for selecting the variant of a RecordWithVariantsType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenu.hpp>

namespace babelwires {

    /// QAction for selecting the variant of a RecordWithVariantsType.
    class SelectVariantAction : public babelwires::NodeContentsContextMenuAction {
      public:
        SelectVariantAction(babelwires::Path pathToRecord, ShortId optional);

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        babelwires::Path m_pathToRecord;
        ShortId m_tag;
    };
}
