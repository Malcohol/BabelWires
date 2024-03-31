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


babelwires::SubtypeOrder babelwires::SumType::compareSubtypeHelper(
    const TypeSystem& typeSystem, const Type& other) const {
    
    const SumType* const otherSumType = other.as<SumType>();
    if (!otherSumType) {
        return SubtypeOrder::IsUnrelated;
    }
    const std::vector<TypeRef>& summandsA = getSummands();
    const std::vector<TypeRef>& summandsB = otherSumType->getSummands();
    
    if ((summandsA.size() <= 2) || (summandsB.size() != summandsA.size())) {
        return SubtypeOrder::IsUnrelated;
    }
    SubtypeOrder order = typeSystem.compareSubtype(summandsA[0], summandsB[0]);
    if (order == SubtypeOrder::IsUnrelated) {
        return SubtypeOrder::IsUnrelated;
    }
    for (int i = 1; i < summandsA.size(); ++i) {
        const SubtypeOrder currentOrder = typeSystem.compareSubtype(summandsA[i], summandsB[i]);
        if ((currentOrder != SubtypeOrder::IsEquivalent) && (currentOrder != order)) {
            if (order == SubtypeOrder::IsEquivalent) {
                order = currentOrder;
            } else {
                return SubtypeOrder::IsUnrelated;
            }
        }
    }
    return order;
}

std::string babelwires::SumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    const int summandIndex = getIndexOfValue(typeSystem, *v);
    const Type& type = m_summands[summandIndex].assertResolve(typeSystem);
    return type.valueToString(typeSystem, v);
}
