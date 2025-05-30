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

babelwires::SubtypeOrder babelwires::SumType::opUnionRight(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder combineTable[5][5] = {
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype },
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting },
        { SubtypeOrder::IsSubtype, SubtypeOrder::IsSubtype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsIntersecting, SubtypeOrder::IsDisjoint }
    };
    return combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
}

babelwires::SubtypeOrder babelwires::SumType::opUnionLeft(SubtypeOrder a, SubtypeOrder b) {
    return reverseSubtypeOrder(
        opUnionRight(reverseSubtypeOrder(a), reverseSubtypeOrder(b)));
}

babelwires::SubtypeOrder babelwires::SumType::opCombine(SubtypeOrder subTest, SubtypeOrder superTest) {
    static constexpr SubtypeOrder error = static_cast<SubtypeOrder>(255);
    static constexpr SubtypeOrder combineTable[5][5] = {
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, error },
        { SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, SubtypeOrder::IsEquivalent, SubtypeOrder::IsSubtype, error },
        { SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, error },
        { SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, SubtypeOrder::IsSupertype, SubtypeOrder::IsIntersecting, error },
        { error, error, error, error, SubtypeOrder::IsDisjoint }
    };
    const auto result = combineTable[static_cast<unsigned int>(subTest)][static_cast<unsigned int>(superTest)];
    assert((result != error) && "Inconsistent result");
    return result;
}

namespace {
    /// Flatten any upper structure of sumtypes into the summand vector
    /// Return false if a subtype did not resolve 
    bool flattenSubSumtypes(const babelwires::TypeSystem& typeSystem, std::vector<babelwires::TypeRef>& summands) {
        unsigned int i = 0;
        while (i < summands.size()) {
            babelwires::TypeRef& summand = summands[i];
            if (const babelwires::Type* summandType = summand.tryResolve(typeSystem)) {
                if (const babelwires::SumType* const subSumType = summandType->as<babelwires::SumType>()) {
                    const std::vector<babelwires::TypeRef>& subSummands = subSumType->getSummands();
                    summand = subSummands[0];
                    summands.insert(summands.end(), subSummands.begin() + 1, subSummands.end());
                } else {
                    ++i;
                }    
            } else {
                // Unresolved subtype
                return false;
            }
        }
        return true;
    }
}

std::optional<babelwires::SubtypeOrder> babelwires::SumType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                  const Type& other) const {
    std::vector<TypeRef> summandsA = getSummands();
    std::vector<TypeRef> summandsB;
    summandsB.emplace_back(other.getTypeRef());
    // Flatten sumtypes into the vector to ensure sumtypes are associative. 
    if (!flattenSubSumtypes(typeSystem, summandsA) || !flattenSubSumtypes(typeSystem, summandsB)) {
        // Unresolved subtype
        return SubtypeOrder::IsDisjoint;
    }

    // TODO
    // 1. Other type is not a sum.
    //   disjoint => {}
    // 2. Other type is a sum.
    //   disjoin => disjoint.

    // Using optionals here as a pseudo-identity for the operators.
    std::optional<SubtypeOrder> subTest;
    std::vector<std::optional<SubtypeOrder>> superTestForBs(summandsB.size());
    for (int i = 0; i < summandsA.size(); ++i) {
        std::optional<SubtypeOrder> subTestForA;
        for (int j = 0; j < summandsB.size(); ++j) {
            const SubtypeOrder ab = typeSystem.compareSubtype(summandsA[i], summandsB[j]);
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

    return opCombine(*subTest, superTest);
}

std::string babelwires::SumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const int summandIndex = getIndexOfValue(typeSystem, *v);
    const Type& type = m_summands[summandIndex].assertResolve(typeSystem);
    return type.valueToString(typeSystem, v);
}
