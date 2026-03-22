/**
 * A dummy value constructed by TypeVariableType.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {

    /// A dummy value constructed by TypeVariableType.
    class BABELWIRESLIB_API TypeVariableValue : public Value {
      public:
        DOWNCASTABLE(TypeVariableValue, Value);
        CLONEABLE(TypeVariableValue);

        TypeVariableValue() = default;

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
    };
}
