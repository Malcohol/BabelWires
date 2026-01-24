/**
 * The Serializer supports the saving of serialized data, where the particular representation (e.g. XML) of data is abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/serializer.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializable.hpp>

#include <cassert>

babelwires::Serializer::~Serializer() {
    assert(((std::uncaught_exceptions() > 0) || m_wasFinalized) && "The serializer was not finalized");
}

void babelwires::Serializer::pushCommon() {
    assert(((m_depth > 0) || m_isEmpty) && "There can only be one outer object or array");
    ++m_depth;
    m_isEmpty = false;
}

void babelwires::Serializer::popCommon() {
    --m_depth;
}

void babelwires::Serializer::pushObject(std::string_view typeName) {
    pushCommon();
    doPushObject(typeName);
}

void babelwires::Serializer::pushObjectWithKey(std::string_view typeName, std::string_view key) {
    pushCommon();
    doPushObjectWithKey(typeName, key);
}

void babelwires::Serializer::popObject() {
    popCommon();
    doPopObject();
}

void babelwires::Serializer::pushArray(std::string_view key) {
    pushCommon();
    doPushArray(key);
}

void babelwires::Serializer::popArray() {
    popCommon();
    doPopArray();
}

void babelwires::Serializer::initialize() {
    pushObject("contents");
}

void babelwires::Serializer::finalize() {
    serializeMetadata(*this);
    popObject();
    m_wasFinalized = true;
}
