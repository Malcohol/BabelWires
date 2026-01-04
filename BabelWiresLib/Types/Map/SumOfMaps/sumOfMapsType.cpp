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
    babelwires::SumType::SummandDefinitions getTypeCombinations(const babelwires::SumType::SummandDefinitions& sourceTypes, const babelwires::SumType::SummandDefinitions& targetTypes) {
        babelwires::SumType::SummandDefinitions combinations;
        combinations.reserve(sourceTypes.size() * targetTypes.size());
        for (const auto& s : sourceTypes) {
            for (const auto& t : targetTypes) {
                combinations.emplace_back(babelwires::TypeExp(babelwires::MapTypeConstructor::getThisIdentifier(), s, t));
            }
        }
        return combinations;
    }
}

babelwires::SumOfMapsType::SumOfMapsType(const TypeSystem& typeSystem, SummandDefinitions sourceTypes, SummandDefinitions targetTypes,
                                         unsigned int indexOfDefaultSourceType, unsigned int indexOfDefaultTargetType)
    : SumType(typeSystem, getTypeCombinations(sourceTypes, targetTypes),
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

const babelwires::SumType::SummandDefinitions& babelwires::SumOfMapsType::getSourceTypes() const {
    return m_sourceTypes;
}
const babelwires::SumType::SummandDefinitions& babelwires::SumOfMapsType::getTargetTypes() const {
    return m_targetTypes;
}
unsigned int babelwires::SumOfMapsType::getIndexOfDefaultSourceType() const {
    return m_indexOfDefaultSourceType;
}
unsigned int babelwires::SumOfMapsType::getIndexOfDefaultTargetType() const {
    return m_indexOfDefaultTargetType;
}
