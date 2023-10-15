/**
 * ArrayTypes have values which contain a dynamically-sized sequence of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Array/arrayValue.hpp>

// TODO Remove
#include <BabelWiresLib/Types/Int/intType.hpp>

babelwires::TestArrayType::TestArrayType()
    : ArrayType(DefaultIntType::getThisIdentifier(), 1, 10, 3) {}

babelwires::TestArrayType2::TestArrayType2()
    : ArrayType(TestArrayType::getThisIdentifier(), 1, 4, 2) {}

babelwires::ArrayType::ArrayType(TypeRef entryType, unsigned int minimumSize, unsigned int maximumSize, int initialSize)
    : m_entryType(std::move(entryType))
    , m_minimumSize(minimumSize)
    , m_maximumSize(maximumSize)
    , m_initialSize((initialSize >= static_cast<int>(minimumSize)) ? initialSize : minimumSize) {
    assert((minimumSize <= maximumSize) && "The minimum is not smaller than the maximum");
    assert((m_initialSize >= minimumSize) && "The initial size is too big");
    assert((m_initialSize <= maximumSize) && "The initial size is too small");
}

const babelwires::TypeRef& babelwires::ArrayType::getEntryType() const {
    return m_entryType;
}

babelwires::NewValueHolder babelwires::ArrayType::createValue(const TypeSystem& typeSystem) const {
    const Type& entryType = m_entryType.resolve(typeSystem);
    return babelwires::ValueHolder::makeValue<ArrayValue>(typeSystem, entryType, m_initialSize);
}

bool babelwires::ArrayType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const ArrayValue* const arrayValue = v.as<ArrayValue>();
    if (!arrayValue) {
        return false;
    }
    const unsigned int size = arrayValue->getSize();
    if ((size < m_minimumSize) || (size > m_maximumSize)) {
        return false;
    }
    const Type& entryType = m_entryType.resolve(typeSystem);
    for (unsigned int i = 0; i < size; ++i) {
        if (!entryType.isValidValue(typeSystem, *arrayValue->getValue(i))) {
            return false;
        }
    }
    return true;
}

std::string babelwires::ArrayType::getKind() const {
    return "array";
}

babelwires::SubtypeOrder babelwires::ArrayType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                     const Type& other) const {
    const ArrayType* const otherArray = other.as<ArrayType>();
    if (!otherArray) {
        return SubtypeOrder::IsUnrelated;
    }
    SubtypeOrder rangeOrder;
    if ((m_minimumSize == otherArray->m_minimumSize) && (m_maximumSize == otherArray->m_maximumSize)) {
        rangeOrder == SubtypeOrder::IsEquivalent;
    } else if ((m_minimumSize >= otherArray->m_minimumSize) && (m_maximumSize <= otherArray->m_maximumSize)) {
        rangeOrder == SubtypeOrder::IsSubtype;
    } else if ((m_minimumSize <= otherArray->m_minimumSize) && (m_maximumSize >= otherArray->m_maximumSize)) {
        rangeOrder == SubtypeOrder::IsSupertype;
    } else {
        return SubtypeOrder::IsUnrelated;
    }

    const SubtypeOrder entryOrder = typeSystem.compareSubtype(m_entryType, otherArray->getEntryType());

    if (entryOrder == rangeOrder) {
        return entryOrder;
    } else if (entryOrder == SubtypeOrder::IsEquivalent) {
        return rangeOrder;
    } else if (rangeOrder == SubtypeOrder::IsEquivalent) {
        return entryOrder;
    } else {
        return SubtypeOrder::IsUnrelated;
    }
}

int babelwires::ArrayType::getNumChildren(const ValueHolder& compoundValue) const {
    const ArrayValue& arrayValue = compoundValue->is<ArrayValue>();
    return arrayValue.getSize();
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&> babelwires::ArrayType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    const ArrayValue& arrayValue = compoundValue->is<ArrayValue>();
    return {&arrayValue.getValue(i), i, m_entryType};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&> babelwires::ArrayType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    ArrayValue& arrayValue = compoundValue.copyContentsAndGetNonConst().is<ArrayValue>();
    return {&arrayValue.getValue(i), i, m_entryType};
}

int babelwires::ArrayType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    const ArrayValue& arrayValue = compoundValue->is<ArrayValue>();
    if (const ArrayIndex* index = step.asIndex()) {
        if (*index < arrayValue.getSize()) {
            return *index;
        }
    }
    return -1;
}
