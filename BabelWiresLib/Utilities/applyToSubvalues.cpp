/**
 * Apply a function to all appropriate subvalues of a given value.
 * 
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#include <BabelWiresLib/Utilities/applyToSubvalues.hpp>

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace {
    // Returns a non-empty ValueHolder if the function was applied to any subvalues.
    babelwires::ValueHolder
    applyToSubvaluesInternal(const babelwires::TypeSystem& typeSystem, const babelwires::Type& type,
                             const babelwires::ValueHolder& sourceValue,
                             const std::function<bool(const babelwires::Type&, const babelwires::Value&)>& predicate,
                             const std::function<void(const babelwires::Type&, babelwires::Value&)>& function) {

        if (predicate(type, *sourceValue)) {
            // If the predicate matches, apply the function to the value.
            babelwires::ValueHolder result = sourceValue;
            auto& nonConstValue = result.copyContentsAndGetNonConst();
            function(type, nonConstValue);
            return result;
        }

        // If the predicate does not match, check if the value is a compound type.
        if (const auto* compoundType = type.as<babelwires::CompoundType>()) {
            // Assigned only if some value within it needs to be modified.
            babelwires::ValueHolder result;

            // Iterate over the children of the compound value.
            unsigned int numChildren = compoundType->getNumChildren(sourceValue);
            for (unsigned int i = 0; i < numChildren; ++i) {
                auto [childValue, step, childType] = compoundType->getChild(sourceValue, i);
                if (auto childResult = applyToSubvaluesInternal(typeSystem, *childType,
                                                                *childValue, predicate, function)) {
                    if (!result) {
                        result = sourceValue;
                        result.copyContentsAndGetNonConst();
                    }
                    // MAYBEDO I think there may be unnecessary clones happening here if there are intermediate compound types.
                    auto [childNonConstValue, step2, childType] = compoundType->getChildNonConst(result, i);
                    *childNonConstValue = std::move(childResult);
                }
            }

            if (result) {
                return result; // Return the modified compound value.
            }
        }
        return {}; // Return an empty ValueHolder if no modifications were made.
    }
} // namespace

void babelwires::applyToSubvalues(const babelwires::TypeSystem& typeSystem, const Type& type, ValueHolder& sourceValue,
                                  const std::function<bool(const Type&, const Value&)>& predicate,
                                  const std::function<void(const Type&, Value&)>& function) {

    if (auto result = applyToSubvaluesInternal(typeSystem, type, sourceValue, predicate, function)) {
        sourceValue = std::move(result); // Update the original ValueHolder with the modified value.
    }
}
