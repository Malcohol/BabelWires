/**
 * Model for MapValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {
    class DataLocation;

    class MapValueModel : ValueModel {
      public:
        void getContextMenuActions(const DataLocation& location, std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const override;
    };
} // namespace babelwires
