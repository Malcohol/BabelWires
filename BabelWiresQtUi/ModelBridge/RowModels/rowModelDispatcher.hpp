/**
 * The RowModelDispatcher provides access to an appropriate RowModel for a row.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class ContentsCacheEntry;
    class FeatureElement;
    class RowModelRegistry;

    /// Dispatches to an appropriate RowModel.
    /// A bit of a hack to avoid an unnecessary allocation:
    /// Allocate the appropriate rowModel on top of its same-sized base class.
    /// Neither require destruction, so it should be safe.
    class RowModelDispatcher {
      public:
        RowModelDispatcher(const RowModelRegistry& rowModelRegistry, const ContentsCacheEntry* entry,
                           const FeatureElement* element);

        const RowModel* operator->() const { return m_rowModel; }

      private:
        RowModel* m_rowModel;
        RowModel m_rowModelStorage;
    };

} // namespace babelwires
