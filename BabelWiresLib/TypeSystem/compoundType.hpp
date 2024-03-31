/**
 * CompoundTypes have values which have a structure made of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/pathStep.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// CompoundTypes have values which have a structure made of child values.
    class CompoundType : public Type {
      public:
        /// Get the number of children in the given value of this type.
        virtual unsigned int getNumChildren(const ValueHolder& compoundValue) const = 0;

        /// Get the ith child of the given value of this type.
        // TODO Clarify failure behaviour. Probably return reference.
        virtual std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue,
                                                                                  unsigned int i) const = 0;

        /// Get a writeable valueHolder to the ith child of the given value of this type.
        /// It is expected this this will shallow clone the given ValueHolder using "copyContentsAndGetNonConst".
        virtual std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue,
                                                                                    unsigned int i) const = 0;

        virtual int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const = 0;
    };
} // namespace babelwires
