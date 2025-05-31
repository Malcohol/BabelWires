/**
 * SumType is the type which allows any value of any of a number of other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

namespace babelwires {

    /// SumType is the type which allows any value of any of a number of other types.
    /// A value of a sum type is just a bare values of one of the summands. 
    /// Be very careful when making a sumtype with overlapping summands, because the
    /// UI cannot know which type is intended and editing won't work properly.
    class SumType : public Type {
      public:
        using Summands = std::vector<TypeRef>;

        SumType(Summands summands, unsigned int indexOfDefaultSummand = 0);

        /// Always creates a value of the first summand.
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        /// Return the index of the first summand of which v is a valid value.
        /// Returns -1 if the value is not valid.
        int getIndexOfValue(const TypeSystem& typeSystem, const Value& v) const;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getFlavour() const override;

        const Summands& getSummands() const;
        unsigned int getIndexOfDefaultSummand() const;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

      public:
        /// If a is the relationship between T and U, and b is the relationship between T and V
        /// then this is the relationship between T and (U + V).
        static SubtypeOrder opUnionRight(SubtypeOrder a, SubtypeOrder b);

        /// If a is the relationship between T and V, and b is the relationship between U and V
        /// then this is the relationship between (T + U) and V.
        static SubtypeOrder opUnionLeft(SubtypeOrder a, SubtypeOrder b);

        /// Combine the result of testing As a subtype of Bs with the result of
        /// testing As a supertype of Bs.
        static SubtypeOrder opCombine(SubtypeOrder subTest, SubtypeOrder superTest);

      private:
        Summands m_summands;
        unsigned int m_indexOfDefaultSummand;
    };
} // namespace babelwires
