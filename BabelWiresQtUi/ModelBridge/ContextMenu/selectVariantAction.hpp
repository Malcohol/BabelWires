/**
 * QAction for activating or deactivating optional fields.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

namespace babelwires {

    /// QAction for activating or deactivating an optional.
    class SelectVariantAction : public babelwires::FeatureContextMenuAction {
      public:
        SelectVariantAction(babelwires::Path pathToRecord, ShortId optional);

        virtual void actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const override;

      private:
        babelwires::Path m_pathToRecord;
        ShortId m_tag;
    };
}
