/**
 * CompoundTypes have values which have a structure made of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/pathStep.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// CompoundTypes have values which have a structure made of child values.
    class CompoundType : public Type {
      public:
        /// Get the number of children in the given value of this type.
        virtual unsigned int getNumChildren(const ValueHolder& compoundValue) const = 0;

        /// Get the ith child of the given value of this type.
        // TODO Clarify failure behaviour. Probably return reference.
        virtual std::tuple<const ValueHolder*, PathStep, const TypeExp&> getChild(const ValueHolder& compoundValue,
                                                                                  unsigned int i) const = 0;

        /// Get a writeable valueHolder to the ith child of the given value of this type.
        /// It is expected this this will shallow clone the given ValueHolder using "copyContentsAndGetNonConst".
        virtual std::tuple<ValueHolder*, PathStep, const TypeExp&> getChildNonConst(ValueHolder& compoundValue,
                                                                                    unsigned int i) const = 0;

        virtual int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const = 0;

        /// Check whether the two values have differences independent of whether their children are different.
        /// This accounts for the fact that a compound type might have some data that isn't entirely defined by its
        /// children. An example is the tag which selects the variants of a recordWithVariantsType.
        /// The default implementation returns false.
        virtual bool areDifferentNonRecursively(const ValueHolder& compoundValueA, const ValueHolder& compoundValueB) const;
    };
} // namespace babelwires
