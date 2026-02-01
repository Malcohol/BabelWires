/**
 * Inline implementations for the Deserializer.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <type_traits>

template <typename V>
std::enable_if_t<
    babelwires::IsSerializableValue<V>::value &&
        std::is_same_v<decltype(V::deserializeFromString(std::declval<std::string>())), babelwires::ResultT<V>>,
    babelwires::ResultT<bool>>
babelwires::Deserializer::tryDeserializeValue(std::string_view key, V& value) {
    std::string asString;
    ResultT<bool> ret = tryDeserializeValue(key, asString);
    if (!ret) {
        return ret;
    }
    if (!*ret) {
        return false;
    }
    ResultT<V> result = V::deserializeFromString(asString);
    if (result) {
        value = *result;
    }
    return result.transform([](const V&) { return true; });
}

template <typename T>
inline babelwires::Result babelwires::Deserializer::deserializeValue(std::string_view key, T& value) {
    const ResultT<bool> result = tryDeserializeValue(key, value);
    if (result) {
        if (*result) {
            return {};
        } else {
            return Error() << "No value for \"" << key << "\" found";
        }
    } else {
        return Error() << "Error when deserializing \"" << key << "\": " << result.error().toString();
    }
}

template <typename T>
inline babelwires::ResultT<std::unique_ptr<T>> babelwires::Deserializer::deserializeCurrentObject() {
    return deserializeCurrentObject(T::getSerializationTag()).transform([](std::unique_ptr<Serializable>&& basePtr) {
        return uniquePtrCast<T>(std::move(basePtr));
    });
}

template <typename T>
inline babelwires::ResultT<std::unique_ptr<T>> babelwires::Deserializer::deserializeObject(std::string_view key) {
    if (!pushObject(key)) {
        return Error() << "Missing child \"" << key << "\"";
    }
    ResultT<std::unique_ptr<T>> ret = deserializeCurrentObject<T>();
    popObject();
    return ret;
}

template<typename T>
inline babelwires::ResultT<std::unique_ptr<T>>
babelwires::Deserializer::tryDeserializeObject(std::string_view key) {
    if (!pushObject(key)) {
        return nullptr;
    }
    ResultT<std::unique_ptr<T>> ret = deserializeCurrentObject<T>();
    popObject();
    return ret;
}

template<typename T>
babelwires::Result babelwires::Deserializer::deserializeObjectByValue(T& object, std::string_view key) {
    // MAYBEDO: Reject strict subclasses, since the slicing will discard data?
    ASSIGN_OR_ERROR(auto objPtr, deserializeObject<T>(key));
    object = std::move(*objPtr);
    return {};    
}

template<typename T>
babelwires::ResultT<bool> babelwires::Deserializer::tryDeserializeObjectByValue(T& object, std::string_view key) {
    // MAYBEDO: Reject strict subclasses, since the slicing will discard data?
    ASSIGN_OR_ERROR(auto objPtr, tryDeserializeObject<T>(key));
    if (!objPtr) {
        return false;
    }
    object = std::move(*objPtr);
    return true;
}

template <typename T> inline babelwires::ResultT<std::unique_ptr<T>> babelwires::Deserializer::Iterator<T>::getObject() {
    return m_deserializer.deserializeCurrentObject<T>();
}

template <typename T>
inline babelwires::Deserializer::Iterator<T>::Iterator(std::unique_ptr<AbstractIterator> impl,
                                                       Deserializer& deserializer)
    : BaseIterator(std::move(impl), deserializer) {}

template <typename T>
inline babelwires::ResultT<typename babelwires::Deserializer::Iterator<T>>
babelwires::Deserializer::tryDeserializeArray(std::string_view key) {
    if (!pushArray(key)) {
        return ResultT<Iterator<T>>(std::in_place, nullptr, *this);
    }
    return ResultT<Iterator<T>>(std::in_place, getIteratorImpl(), *this);
}

template <typename T>
inline babelwires::ResultT<typename babelwires::Deserializer::Iterator<T>>
babelwires::Deserializer::deserializeArray(std::string_view key) {
    if (!pushArray(key)) {
        return Error() << "Missing child \"" + std::string(key) + "\"";
    }
    return ResultT<Iterator<T>>(std::in_place, getIteratorImpl(), *this);
}

template <typename T> inline T babelwires::Deserializer::ValueIterator<T>::deserializeValue(T tempValue) {
    auto result = m_deserializer.deserializeValue("value", tempValue);
    THROW_ON_ERROR(result, ParseException);
    return tempValue;
}

template <typename T>
inline babelwires::Deserializer::ValueIterator<T>::ValueIterator(std::unique_ptr<AbstractIterator> impl,
                                                                 Deserializer& deserializer, std::string_view typeName)
    : BaseIterator(std::move(impl), deserializer, std::string(typeName)) {}

template <typename T>
inline babelwires::ResultT<typename babelwires::Deserializer::ValueIterator<T>>
babelwires::Deserializer::tryDeserializeValueArray(std::string_view key, std::string_view typeName) {
    if (!pushArray(key)) {
        return ResultT<ValueIterator<T>>(std::in_place, nullptr, *this);
    }
    return ResultT<ValueIterator<T>>(std::in_place, getIteratorImpl(), *this, typeName);
}

template <typename T>
inline babelwires::ResultT<typename babelwires::Deserializer::ValueIterator<T>>
babelwires::Deserializer::deserializeValueArray(std::string_view key, std::string_view typeName) {
    if (!pushArray(key)) {
        return Error() << "Missing child \"" + std::string(key) + "\"";
    }
    return ResultT<ValueIterator<T>>(std::in_place, getIteratorImpl(), *this, typeName);
}
