/**
 * SumType is the type which allows any value of any of a number of other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

namespace babelwires {

    /// SumType is the type which allows any value of any of a number of other types.
    /// The values are SumValues, which are values tagged by their type.
    /// Implementation note: Bare values would work (we could probably allow ambiguity) but testing
    /// whether a value is a member of the sum gets computationally painful.
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
