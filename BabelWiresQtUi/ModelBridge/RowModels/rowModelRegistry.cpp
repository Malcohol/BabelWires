/**
 * Allows the set of RowModels to be extended by client code.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelRegistry.hpp>

bool babelwires::RowModelRegistry::handleFeature(const Feature* feature, RowModel* rowModelAllocation) const {
    for (const auto& h : m_handlers) {
        if (h(feature, rowModelAllocation)) {
            return true;
        }
    }
    return false;
}
