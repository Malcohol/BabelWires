/**
 * A sum of maps whose source types are picked from one set and whose target types are picked from another.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Sum/sumType.hpp>

namespace babelwires {
    /// A sum type whose summands are map types, where the source types are picked from one set and the target types are
    /// picked from another. As far as the type system goes, this isn't a separate kind of type: it's just a subset of
    /// sum types. However, we special-case it in the UI to give it the same UI as we do for Map types. It lets the user
    /// select type options from within the MapEditor.
    ///
    /// Implementation note: The sum has every combination of source and target types, which is a bit unfortunate. I'm
    /// sure there is a much cleaner way of providing the same functionality, but this will have to do for now.
    class SumOfMapsType : public SumType {
      public:
        SumOfMapsType(const TypeSystem& typeSystem, SummandTypeExps sourceTypes, SummandTypeExps targetTypes,
                      unsigned int indexOfDefaultSourceType = 0, unsigned int indexOfDefaultTargetType = 0);

        const SummandTypeExps& getSourceTypes() const;
        const SummandTypeExps& getTargetTypes() const;
        unsigned int getIndexOfDefaultSourceType() const;
        unsigned int getIndexOfDefaultTargetType() const;

        std::tuple<unsigned int, unsigned int> getIndexOfSourceAndTarget(unsigned int indexInSum) const;

      public:
        SummandTypeExps m_sourceTypes;
        SummandTypeExps m_targetTypes;
        unsigned int m_indexOfDefaultSourceType;
        unsigned int m_indexOfDefaultTargetType;
    };
} // namespace babelwires
