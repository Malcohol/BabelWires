/**
 * SumType the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumType.hpp>

babelwires::SumType::SumType(Summands summands)
    : m_summands(summands)
{
}

babelwires::NewValueHolder babelwires::SumType::createValue(const TypeSystem& typeSystem) const 
{
    return m_summands[0].resolve(typeSystem).createValue(typeSystem);
}

bool babelwires::SumType::isValidValue(const TypeSystem& typeSystem, const Value& v) const 
{
    return std::find_if(m_summands.cbegin(), m_summands.cend(), [&typeSystem, &v](const TypeRef& summand)
    {
        return summand.resolve(typeSystem).isValidValue(typeSystem, v);
    }) != m_summands.cend();
};

std::string babelwires::SumType::getKind() const  
{
    return "sum";
}

const babelwires::SumType::Summands& babelwires::SumType::getSummands() const 
{
    return m_summands;
}
