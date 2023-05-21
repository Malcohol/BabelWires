/**
 * SumType the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Sum/sumValue.hpp>

babelwires::SumType::SumType(Summands summands)
    : m_summands(std::move(summands)) {}

babelwires::NewValueHolder babelwires::SumType::createValue(const TypeSystem& typeSystem) const {
    const unsigned int defaultType = 0;
    return babelwires::ValueHolder::makeValue<SumValue>(
        m_summands[defaultType], m_summands[defaultType].resolve(typeSystem).createValue(typeSystem).is<EditableValue>());
}

bool babelwires::SumType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const SumValue* const sumValue = v.as<SumValue>()) {
        const TypeRef& typeTag = sumValue->getTypeTag();
        // Check that the value is actually a value of its tag type.
        if (typeTag.resolve(typeSystem).isValidValue(typeSystem, *sumValue->getValue())) {
            // Look for the type in the summands (common case).
            if (std::find(m_summands.cbegin(), m_summands.cend(), typeTag) != m_summands.end()) {
                return true;
            }
            // See if typeTag is a subtype of one of the summands.
            // Note: A value might be a member of one of the summands but its tag means it is not.
            // Not sure if that's a good thing or not.
            return std::find_if(m_summands.cbegin(), m_summands.cend(),
                                [&typeSystem, &typeTag](const TypeRef& summand) {
                                    return typeSystem.compareSubtype(typeTag, summand) == SubtypeOrder::IsSubtype;
                                }) != m_summands.cend();
        }
    }
    return false;
};

std::string babelwires::SumType::getKind() const {
    // Would be better if this was somehow the union of the kinds of the summands.
    return "sum";
}

const babelwires::SumType::Summands& babelwires::SumType::getSummands() const {
    return m_summands;
}
