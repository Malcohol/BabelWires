/**
 * The row model for ArrayFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class ArrayFeature;
    class FeatureContextMenuAction;

    /// The row model for ArrayFeatures.
    class ArrayRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual void
        getContextMenuActions(std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const override;

      public:
        const ArrayFeature& getArrayFeature() const;
    };

} // namespace babelwires
