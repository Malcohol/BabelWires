/**
 * SumType the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SumType::SumType(Summands summands, unsigned int indexOfDefaultSummand)
    : m_summands(std::move(summands))
    , m_indexOfDefaultSummand(indexOfDefaultSummand) {
    assert(indexOfDefaultSummand < m_summands.size());
}

babelwires::NewValueHolder babelwires::SumType::createValue(const TypeSystem& typeSystem) const {
    return m_summands[m_indexOfDefaultSummand].resolve(typeSystem).createValue(typeSystem);
}

int babelwires::SumType::getIndexOfValue(const TypeSystem& typeSystem, const Value& v) const {
    const auto it = std::find_if(m_summands.cbegin(), m_summands.cend(), [&typeSystem, &v](const TypeRef& summand) {
               return summand.resolve(typeSystem).isValidValue(typeSystem, v);
           });
    return (it != m_summands.cend()) ? std::distance(m_summands.cbegin(), it) : -1;
}

bool babelwires::SumType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    return getIndexOfValue(typeSystem, v) != -1;
};

std::string babelwires::SumType::getKind() const {
    // Would be better if this was somehow the union of the kinds of the summands.
    return "sum";
}

const babelwires::SumType::Summands& babelwires::SumType::getSummands() const {
    return m_summands;
}

unsigned int babelwires::SumType::getIndexOfDefaultSummand() const {
    return m_indexOfDefaultSummand;
}


std::optional<babelwires::SubtypeOrder> babelwires::SumType::compareSubtypeHelper(
    const TypeSystem& typeSystem, const Type& other) const {

    const std::vector<TypeRef>& summandsA = getSummands();
    
    // Two cases:
    // 1. Other type is not a sum. Compare it to each summand.
    //   no result => return {}
    // 2. Other type is a sum. Compare summands pairwise.
    //   no result => return unrelated.

    const SumType* const otherSumType = other.as<SumType>();
    // If other is not a sumtype, we treat it as a sum type with one summand.
    std::vector<TypeRef> summandsForNonSumType;
    if (!otherSumType) {
        summandsForNonSumType.emplace_back(other.getTypeRef());
    }
    const std::vector<TypeRef>& summandsB = otherSumType ? otherSumType->getSummands() : summandsForNonSumType;
    
    SubtypeOrder order = SubtypeOrder::IsDisjoint;
    for (int i = 0; i < summandsA.size(); ++i) {
        for (int j = 0; j < summandsB.size(); ++j) {
            const SubtypeOrder summandOrder = typeSystem.compareSubtype(summandsA[i], summandsB[i]);
            order = subtypeSum(order, summandOrder);
        }
    }
    return order;
}

std::string babelwires::SumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    const int summandIndex = getIndexOfValue(typeSystem, *v);
    const Type& type = m_summands[summandIndex].assertResolve(typeSystem);
    return type.valueToString(typeSystem, v);
}
