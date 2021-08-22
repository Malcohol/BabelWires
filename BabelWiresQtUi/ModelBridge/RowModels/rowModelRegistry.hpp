/**
 * Allows the set of RowModels to be extended by client code.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/features.hpp"

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"

#include <functional>
#include <vector>

namespace babelwires {
    class RowModel;

    /// Allows the set of RowModels to be extended by client code.
    /// This will be required by any domain specific value type which wants a specialized display or editor.
    // TODO This doesn't use the Registry pattern from common. Should it?
    class RowModelRegistry {
      public:
        using Handler = std::function<bool(const Feature*, RowModel*)>;

        template <typename FEATURE_TYPE, typename ROW_MODEL_TYPE> void registryHandler() {
            m_handlers.emplace_back([](const Feature* feature, RowModel* rowModelAllocation) {
                if (feature->template as<FEATURE_TYPE>()) {
                    static_assert(sizeof(ROW_MODEL_TYPE) <= sizeof(babelwires::RowModel));
                    new (rowModelAllocation) ROW_MODEL_TYPE();
                    return true;
                }
                return false;
            });
        }

        /// Return true if this feature could be handled by one of the registered handlers.
        bool handleFeature(const Feature* feature, RowModel* rowModelAllocation) const;

      private:
        std::vector<Handler> m_handlers;
    };

} // namespace babelwires
