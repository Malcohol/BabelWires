/**
 * Apply a function to all appropriate subvalues of a given value.
 * 
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    /// Apply a function to all appropriate subvalues of a given value.
    /// If the predicate returns true for a subvalue, the function is applied to that subvalue.
    /// However, the function is not applied recursively to the subvalues of that subvalue.
    void applyToSubvalues(const TypeSystem& typeSystem, const Type& type, ValueHolder& sourceValue,
                          const std::function<bool(const Type&, const Value&)>& predicate,
                          const std::function<void(const Type&, Value&)>& function);

    /// A convenience function to apply a function to all subvalues of a given type.
    template <typename TYPE_CLASS>
    void applyToSubvaluesOfType(const TypeSystem& typeSystem, const Type& type, ValueHolder& sourceValue,
                                const std::function<void(const Type&, Value&)>& function) {
        applyToSubvalues(
            typeSystem, type, sourceValue, [](const Type& t, const Value& v) { return t.tryAs<TYPE_CLASS>(); }, function);
    }

} // namespace babelwires
