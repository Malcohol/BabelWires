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
    class SumType : public Type {
      public:
        using Summands = std::vector<TypeRef>;

        SumType(Summands summands, unsigned int indexOfDefaultSummand = 0);

        /// Always creates a value of the first summand.
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        const Summands& getSummands() const;
        unsigned int getIndexOfDefaultSummand() const;

      private:
        Summands m_summands;
        unsigned int m_indexOfDefaultSummand;
    };
} // namespace babelwires
