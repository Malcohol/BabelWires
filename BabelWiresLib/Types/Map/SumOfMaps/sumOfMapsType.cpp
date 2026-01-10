/**
 * SumType the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>

#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>

namespace {
    babelwires::SumType::SummandTypeExps getTypeCombinations(const babelwires::SumType::SummandTypeExps& sourceTypes, const babelwires::SumType::SummandTypeExps& targetTypes) {
        babelwires::SumType::SummandTypeExps combinations;
        combinations.reserve(sourceTypes.size() * targetTypes.size());
        for (const auto& s : sourceTypes) {
            for (const auto& t : targetTypes) {
                combinations.emplace_back(babelwires::TypeExp(babelwires::MapTypeConstructor::getThisIdentifier(), s, t));
            }
        }
        return combinations;
    }
}

babelwires::SumOfMapsType::SumOfMapsType(TypeExp typeExp, const TypeSystem& typeSystem, SummandTypeExps sourceTypes, SummandTypeExps targetTypes,
                                         unsigned int indexOfDefaultSourceType, unsigned int indexOfDefaultTargetType)
    : SumType(std::move(typeExp), typeSystem, getTypeCombinations(sourceTypes, targetTypes),
              (indexOfDefaultSourceType * sourceTypes.size()) + indexOfDefaultTargetType)
    , m_sourceTypes(std::move(sourceTypes))
    , m_targetTypes(std::move(targetTypes))
    , m_indexOfDefaultSourceType(indexOfDefaultSourceType)
    , m_indexOfDefaultTargetType(indexOfDefaultTargetType)
{
    assert(indexOfDefaultSourceType < m_sourceTypes.size());
    assert(indexOfDefaultTargetType < m_targetTypes.size());
}

std::tuple<unsigned int, unsigned int> babelwires::SumOfMapsType::getIndexOfSourceAndTarget(unsigned int indexInSum) const {
    const unsigned int numSourceTypes = m_sourceTypes.size();
    assert(indexInSum < numSourceTypes * m_targetTypes.size());
    return { indexInSum / numSourceTypes, indexInSum % numSourceTypes};
}

const babelwires::SumType::SummandTypeExps& babelwires::SumOfMapsType::getSourceTypes() const {
    return m_sourceTypes;
}
const babelwires::SumType::SummandTypeExps& babelwires::SumOfMapsType::getTargetTypes() const {
    return m_targetTypes;
}
unsigned int babelwires::SumOfMapsType::getIndexOfDefaultSourceType() const {
    return m_indexOfDefaultSourceType;
}
unsigned int babelwires::SumOfMapsType::getIndexOfDefaultTargetType() const {
    return m_indexOfDefaultTargetType;
}
