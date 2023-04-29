/**
 * Allows the set of RowModels to be extended by client code.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/valueModelRegistry.hpp>

bool babelwires::ValueModelRegistry::handleFeature(const Type* type, ValueModel* valueModelAllocation) const {
    for (const auto& h : m_handlers) {
        if (h(type, valueModelAllocation)) {
            return true;
        }
    }
    return false;
}
