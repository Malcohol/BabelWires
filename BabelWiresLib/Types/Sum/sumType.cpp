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

babelwires::SubtypeOrder babelwires::SumType::opInner(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder combineTable[5][5] = {
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype },
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting, SubtypeOrder::IsDisjoint }
    };
    return combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
}

babelwires::SubtypeOrder babelwires::SumType::opOuter(SubtypeOrder a, SubtypeOrder b) {
    return reverseSubtypeOrder(
        opInner(reverseSubtypeOrder(a), reverseSubtypeOrder(b)));
}

babelwires::SubtypeOrder babelwires::SumType::opCombine(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder inconsistent = static_cast<SubtypeOrder>(255);
    static constexpr SubtypeOrder combineTable[5][5] = {
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, inconsistent },
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, inconsistent },
        { SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, inconsistent },
        { SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, inconsistent },
        { inconsistent, inconsistent, inconsistent, inconsistent, SubtypeOrder::IsDisjoint }
    };
    const auto result = combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
    assert((result != inconsistent) && "Inconsistent result");
    return result;
}

std::optional<babelwires::SubtypeOrder> babelwires::SumType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                  const Type& other) const {

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

    std::optional<SubtypeOrder> aOrder;
    std::vector<std::optional<SubtypeOrder>> summandBOrders;
    summandBOrders.resize(summandsB.size());
    for (int i = 0; i < summandsA.size(); ++i) {
        std::optional<SubtypeOrder> summandAOrder;
        for (int j = 0; j < summandsB.size(); ++j) {
            const SubtypeOrder summandABOrder = typeSystem.compareSubtype(summandsA[i], summandsB[j]);
            auto& summandBOrder = summandBOrders[j];
            summandAOrder = summandAOrder ? opInner(summandABOrder, *summandAOrder) : summandABOrder;
            summandBOrder = summandBOrder ? opOuter(summandABOrder, *summandBOrder) : summandABOrder;
        }
        aOrder = aOrder ? opOuter(*summandAOrder, *aOrder) : summandAOrder;
    }
    assert(aOrder && "Optional should be resolved here");
    SubtypeOrder bOrder = *summandBOrders[0];
    for (int j = 1; j < summandsB.size(); ++j) {
        bOrder = opInner(*summandBOrders[j], bOrder);
    }

    return opCombine(*aOrder, bOrder);
}

std::string babelwires::SumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const int summandIndex = getIndexOfValue(typeSystem, *v);
    const Type& type = m_summands[summandIndex].assertResolve(typeSystem);
    return type.valueToString(typeSystem, v);
}
