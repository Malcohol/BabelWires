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

bool babelwires::SumType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    return std::find_if(m_summands.cbegin(), m_summands.cend(), [&typeSystem, &v](const TypeRef& summand) {
               return summand.resolve(typeSystem).isValidValue(typeSystem, v);
           }) != m_summands.cend();
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
