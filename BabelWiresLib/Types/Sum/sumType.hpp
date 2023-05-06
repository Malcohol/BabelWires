/**
 * SumType is the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

namespace babelwires {

    /// 
    class SumType : public Type {
      public:
        using Summands = std::vector<TypeRef>;

        SumType(Summands summands);

        /// Always creates a SumValue containing a value of the first summand.
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        const Summands& getSummands() const;

      private:
        Summands m_summands;
    };
} // namespace babelwires
