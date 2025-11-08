/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <BabelWiresLib/TypeSystem/editableValue.hpp>

babelwires::Value& babelwires::ValueHolder::copyContentsAndGetNonConst() {
    std::shared_ptr<Value> clone = m_pointerToValue->is<Value>().cloneShared();
    Value* ptrToClone = clone.get();
    m_pointerToValue = clone;
    return *ptrToClone;
}

void babelwires::ValueHolder::visitIdentifiers(IdentifierVisitor& visitor) {
    if (const EditableValue* editableValue = m_pointerToValue ? m_pointerToValue->tryGetAsEditableValue() : nullptr) {
        if (editableValue->canContainIdentifiers()) {
            copyContentsAndGetNonConst().getAsEditableValue().visitIdentifiers(visitor);
        }
    }
}

void babelwires::ValueHolder::visitFilePaths(FilePathVisitor& visitor) {
    if (const EditableValue* editableValue = m_pointerToValue ? m_pointerToValue->tryGetAsEditableValue() : nullptr) {
        if (editableValue->canContainFilePaths()) {
            copyContentsAndGetNonConst().getAsEditableValue().visitFilePaths(visitor);
        }
    }
}


const babelwires::Value* babelwires::ValueHolder::getUnsafe() const {
    return &m_pointerToValue->is<Value>();
}
