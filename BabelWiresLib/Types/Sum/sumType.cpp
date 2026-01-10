/**
 * SumType the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumType.hpp>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SumType::SumType(TypeExp&& typeExp, const TypeSystem& typeSystem, SummandTypeExps summands, unsigned int indexOfDefaultSummand)
    : Type(std::move(typeExp))
    , m_indexOfDefaultSummand(indexOfDefaultSummand) {
    m_summands.reserve(summands.size());
    for (const auto& s : summands) {
        m_summands.emplace_back(s.resolve(typeSystem));
    }
    assert(indexOfDefaultSummand < m_summands.size());
}

babelwires::SumType::SumType(TypeExp&& typeExp, Summands summands, unsigned int indexOfDefaultSummand)
    : Type(std::move(typeExp))
    , m_summands(std::move(summands))
    , m_indexOfDefaultSummand(indexOfDefaultSummand) {
    assert(indexOfDefaultSummand < m_summands.size());
}

babelwires::NewValueHolder babelwires::SumType::createValue(const TypeSystem& typeSystem) const {
    return m_summands[m_indexOfDefaultSummand]->createValue(typeSystem);
}

int babelwires::SumType::getIndexOfValue(const TypeSystem& typeSystem, const Value& v) const {
    const auto it = std::find_if(m_summands.cbegin(), m_summands.cend(), [&typeSystem, &v](const TypePtr& summand) {
        return summand->isValidValue(typeSystem, v);
    });
    return (it != m_summands.cend()) ? std::distance(m_summands.cbegin(), it) : -1;
}

bool babelwires::SumType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    const int index = getIndexOfValue(typeSystem, v);
    if (index == -1) {
        return false;
    }
    const TypePtr& summandType = m_summands[static_cast<unsigned int>(index)];
    return visitor(typeSystem, summandType, v, PathStep{});
}

std::string babelwires::SumType::getFlavour() const {
    // TODO Maybe better to concatenate the summand kinds?
    return "sum";
}

const babelwires::SumType::Summands& babelwires::SumType::getSummands() const {
    return m_summands;
}

unsigned int babelwires::SumType::getIndexOfDefaultSummand() const {
    return m_indexOfDefaultSummand;
}

babelwires::SubtypeOrder babelwires::SumType::opUnionRight(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder combineTable[5][5] = {
        {SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype,
         SubtypeOrder::IsSubtype},
        {SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype,
         SubtypeOrder::IsSubtype},
        {SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting,
         SubtypeOrder::IsIntersecting},
        {SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting,
         SubtypeOrder::IsIntersecting},
        {SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting,
         SubtypeOrder::IsDisjoint}};
    return combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
}

babelwires::SubtypeOrder babelwires::SumType::opUnionLeft(SubtypeOrder a, SubtypeOrder b) {
    return reverseSubtypeOrder(opUnionRight(reverseSubtypeOrder(a), reverseSubtypeOrder(b)));
}

babelwires::SubtypeOrder babelwires::SumType::opCombine(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder error = static_cast<SubtypeOrder>(255);
    // The asymmetry arises because the function arguments have an asymmetric semantics: The subtest carrying
    // an IsSupertype value is a weaker input than the subtest carrying an IsSubtype value.
    static constexpr SubtypeOrder combineTable[5][5] = {
        {SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype,
         error},
        {SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype,
         error},
        {SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype,
         SubtypeOrder::IsIntersecting, error},
        {SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype,
         SubtypeOrder::IsIntersecting, error},
        {error, error, error, error, SubtypeOrder::IsDisjoint}};
    const auto result = combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
    assert((result != error) && "Inconsistent result");
    return result;
}

namespace {
    /// Flatten any upper structure of sumtypes into the summand vector
    /// Return false if a subtype did not resolve
    void flattenSubSumtypes(const babelwires::TypeSystem& typeSystem, std::vector<const babelwires::Type*>& summands) {
        unsigned int i = 0;
        while (i < summands.size()) {
            const babelwires::Type* summand = summands[i];
            if (const babelwires::SumType* const subSumType = summand->as<babelwires::SumType>()) {
                const std::vector<babelwires::TypePtr>& subSummands = subSumType->getSummands();
                summands[i] = subSummands[0].get();
                summands.reserve(summands.size() + subSummands.size() - 1);
                for (unsigned int i = 1; i < subSummands.size(); ++i) {
                    summands.emplace_back(subSummands[i].get());
                }
            } else {
                ++i;
            }
        }
    }
} // namespace

std::optional<babelwires::SubtypeOrder> babelwires::SumType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                  const Type& other) const {
    std::vector<const Type*> summandsA;
    std::vector<const Type*> summandsB;
    summandsA.emplace_back(this);
    summandsB.emplace_back(&other);
    // Flatten sumtypes into the vector to ensure sumtypes are associative.
    flattenSubSumtypes(typeSystem, summandsA);
    flattenSubSumtypes(typeSystem, summandsB);
    
    // Using optionals here as a pseudo-identity for the operators.
    std::optional<SubtypeOrder> subTest;
    std::vector<std::optional<SubtypeOrder>> superTestForBs(summandsB.size());
    for (int i = 0; i < summandsA.size(); ++i) {
        std::optional<SubtypeOrder> subTestForA;
        for (int j = 0; j < summandsB.size(); ++j) {
            const SubtypeOrder ab = typeSystem.compareSubtype(summandsA[i]->getTypeExp(), summandsB[j]->getTypeExp());
            auto& superTestForB = superTestForBs[j];
            subTestForA = subTestForA ? opUnionRight(ab, *subTestForA) : ab;
            superTestForB = superTestForB ? opUnionLeft(ab, *superTestForB) : ab;
        }
        subTest = subTest ? opUnionLeft(*subTestForA, *subTest) : subTestForA;
    }
    assert(subTest && "Optional should be resolved here");
    SubtypeOrder superTest = *superTestForBs[0];
    for (int j = 1; j < summandsB.size(); ++j) {
        assert(superTestForBs[j] && "Optional should be resolved here");
        superTest = opUnionRight(*superTestForBs[j], superTest);
    }

    const SubtypeOrder result = opCombine(*subTest, superTest);
    if (result == SubtypeOrder::IsDisjoint) {
        if (other.as<SumType>()) {
            // Definitely disjoint
            return SubtypeOrder::IsDisjoint;
        } else {
            // Maybe disjoint, but should allow the other type the opportunity to judge the comparison.
            return {};
        }
    } else {
        return result;
    }
}

std::string babelwires::SumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const int summandIndex = getIndexOfValue(typeSystem, *v);
    return m_summands[summandIndex]->valueToString(typeSystem, v);
}
