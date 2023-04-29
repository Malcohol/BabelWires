/**
 * Allows the set of RowModels to be extended by client code.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

#include <functional>
#include <vector>

namespace babelwires {
    class RowModel;

    /// Allows the set of RowModels to be extended by client code.
    /// This will be required by any domain specific value type which wants a specialized display or editor.
    // TODO This doesn't use the Registry pattern from common. Should it?
    class ValueModelRegistry {
      public:
        using Handler = std::function<bool(const Type*, ValueModel*)>;

        template <typename TYPE, typename VALUE_MODEL_TYPE> void registryHandler() {
            m_handlers.emplace_back([](const Type* type, ValueModel* valueModelAllocation) {
                if (type->template as<TYPE>()) {
                    static_assert(sizeof(VALUE_MODEL_TYPE) <= sizeof(babelwires::ValueModel));
                    new (valueModelAllocation) VALUE_MODEL_TYPE();
                    return true;
                }
                return false;
            });
        }

        /// Return true if this feature could be handled by one of the registered handlers.
        bool handleFeature(const Type* type, ValueModel* valueModelAllocation) const;

      private:
        std::vector<Handler> m_handlers;
    };

} // namespace babelwires
