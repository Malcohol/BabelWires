/**
 * The row model for RecordWithOptionalsFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"

namespace babelwires {

    class RecordWithOptionalsFeature;
    class FeatureContextMenuAction;

    /// The row model for ArrayFeatures.
    class RecordWithOptionalsRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual void
        getContextMenuActions(std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const override;

      public:
        const babelwires::RecordWithOptionalsFeature& getRecordWithOptionalsFeature() const;
    };

} // namespace babelwires
