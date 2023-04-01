/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

babelwires::ValueHolder::ValueHolder(const ValueHolder& other)
    : m_pointerToValue(other.m_pointerToValue)
{
}

babelwires::ValueHolder::ValueHolder(ValueHolder&& other)
    : m_pointerToValue(std::move(other.m_pointerToValue))
{
}

babelwires::ValueHolder::ValueHolder(const Value& value)
    : m_pointerToValue(value.clone().release())
{
}

babelwires::ValueHolder::ValueHolder(Value&& value)
    // R-value cloning uses the move contructor.    
    : m_pointerToValue(std::move(value).clone().release())
{
}

babelwires::ValueHolder::ValueHolder(std::unique_ptr<Value> ptr)
    : m_pointerToValue(ptr.release())
{
}

babelwires::ValueHolder::ValueHolder(std::shared_ptr<const Value> ptr)
    : m_pointerToValue(std::move(ptr))
{
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(const ValueHolder& other)
{
    m_pointerToValue = other.m_pointerToValue;
    return *this;
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(ValueHolder&& other)
{
    m_pointerToValue = std::move(other.m_pointerToValue);
    return *this;
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(const Value& value)
{
    m_pointerToValue = std::shared_ptr<const Value>(value.clone().release());
    return *this;
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(Value&& value)
{
    // R-value cloning uses the move contructor.
    m_pointerToValue = std::shared_ptr<const Value>(std::move(value).clone().release());
    return *this;
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(std::shared_ptr<const Value> ptr)
{
    m_pointerToValue = std::move(ptr);
    return *this;
}

babelwires::ValueHolder& babelwires::ValueHolder::operator=(std::unique_ptr<Value> ptr)
{
    m_pointerToValue = std::shared_ptr<const Value>(ptr.release());
    return *this;
}

babelwires::ValueHolder::operator bool() const {
    return m_pointerToValue.get();
}

const babelwires::Value& babelwires::ValueHolder::operator*() const
{
    return *m_pointerToValue;
}

const babelwires::Value* babelwires::ValueHolder::operator->() const
{
    return m_pointerToValue.get();
}

babelwires::Value& babelwires::ValueHolder::copyContentsAndGetNonConst()
{
    std::unique_ptr<Value> clone = m_pointerToValue->clone();
    Value* ptrToClone = clone.get();
    m_pointerToValue = std::shared_ptr<const Value>(clone.release());
    return *ptrToClone;
}
