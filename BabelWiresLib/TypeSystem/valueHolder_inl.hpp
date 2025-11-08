/**
 * A ValueHolderTemplate is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(const ValueHolderTemplate& other)
    : m_pointerToValue(other.m_pointerToValue) {}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(ValueHolderTemplate&& other)
    : m_pointerToValue(std::move(other.m_pointerToValue)) {}

template <typename VALUE>
template <typename OTHER>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(const ValueHolderTemplate<OTHER>& other)
    : m_pointerToValue(other->template is<VALUE>().cloneShared()) {}

template <typename VALUE>
template <typename OTHER>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(ValueHolderTemplate<OTHER>&& other)
    : m_pointerToValue(std::static_pointer_cast<const VALUE>(std::move(other.m_pointerToValue))) {
    assert(m_pointerToValue->as<VALUE>());
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(const VALUE& value)
    : m_pointerToValue(value.cloneShared()) {}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(VALUE&& value)
    // R-value cloning uses the move contructor.
    : m_pointerToValue(std::move(value).cloneShared()) {}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(std::unique_ptr<VALUE> ptr)
    : m_pointerToValue(ptr.release()) {}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>::ValueHolderTemplate(std::shared_ptr<const VALUE> ptr)
    : m_pointerToValue(std::move(ptr)) {}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>&
babelwires::ValueHolderTemplate<VALUE>::operator=(const ValueHolderTemplate& other) {
    m_pointerToValue = other.m_pointerToValue;
    return *this;
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>& babelwires::ValueHolderTemplate<VALUE>::operator=(ValueHolderTemplate&& other) {
    m_pointerToValue = std::move(other.m_pointerToValue);
    return *this;
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>& babelwires::ValueHolderTemplate<VALUE>::operator=(const VALUE& value) {
    m_pointerToValue = value.cloneShared();
    return *this;
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>& babelwires::ValueHolderTemplate<VALUE>::operator=(VALUE&& value) {
    // R-value cloning uses the move contructor.
    m_pointerToValue = std::move(value).cloneShared();
    return *this;
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>&
babelwires::ValueHolderTemplate<VALUE>::operator=(std::shared_ptr<const VALUE> ptr) {
    m_pointerToValue = std::move(ptr);
    return *this;
}

template <typename VALUE>
babelwires::ValueHolderTemplate<VALUE>& babelwires::ValueHolderTemplate<VALUE>::operator=(std::unique_ptr<VALUE> ptr) {
    m_pointerToValue = std::shared_ptr<const VALUE>(ptr.release());
    return *this;
}

template <typename VALUE> babelwires::ValueHolderTemplate<VALUE>::operator bool() const {
    return m_pointerToValue.get();
}

template <typename VALUE> void babelwires::ValueHolderTemplate<VALUE>::clear() {
    m_pointerToValue = nullptr;
}

template <typename VALUE> const VALUE& babelwires::ValueHolderTemplate<VALUE>::operator*() const {
    return m_pointerToValue->is<VALUE>();
}

template <typename VALUE> const VALUE* babelwires::ValueHolderTemplate<VALUE>::operator->() const {
    return &m_pointerToValue->is<VALUE>();
}

template <typename VALUE> void babelwires::ValueHolderTemplate<VALUE>::swap(ValueHolderTemplate& other) {
    m_pointerToValue.swap(other.m_pointerToValue);
}

template <typename VALUE> VALUE& babelwires::ValueHolderTemplate<VALUE>::copyContentsAndGetNonConst() {
    // TODO Is there an optimization when we _know_ it isn't shared? See the non-const FeaturePath::follow
    std::shared_ptr<VALUE> clone = m_pointerToValue->is<VALUE>().cloneShared();
    VALUE* ptrToClone = clone.get();
    m_pointerToValue = clone;
    return *ptrToClone;
}

template <typename VALUE> void babelwires::ValueHolderTemplate<VALUE>::visitIdentifiers(IdentifierVisitor& visitor) {
    if (const EditableValue* editableValue = m_pointerToValue ? m_pointerToValue->tryGetAsEditableValue() : nullptr) {
        if (editableValue->canContainIdentifiers()) {
            copyContentsAndGetNonConst().getAsEditableValue().visitIdentifiers(visitor);
        }
    }
}

template <typename VALUE> void babelwires::ValueHolderTemplate<VALUE>::visitFilePaths(FilePathVisitor& visitor) {
    if (const EditableValue* editableValue = m_pointerToValue ? m_pointerToValue->tryGetAsEditableValue() : nullptr) {
        if (editableValue->canContainFilePaths()) {
            copyContentsAndGetNonConst().getAsEditableValue().visitFilePaths(visitor);
        }
    }
}

template <typename VALUE> const VALUE* babelwires::ValueHolderTemplate<VALUE>::getUnsafe() const {
    return &m_pointerToValue->is<VALUE>();
}

template <typename VALUE>
template <typename OTHER>
babelwires::ValueHolderTemplate<VALUE>::operator const ValueHolderTemplate<OTHER>&() const {
    assert(m_pointerToValue->as<OTHER>());
    return *reinterpret_cast<const ValueHolderTemplate<OTHER>*>(this);
}

template <typename VALUE>
template <typename DERIVED>
babelwires::ValueHolderTemplate<DERIVED> babelwires::ValueHolderTemplate<VALUE>::asValueHolder() const {
    return ValueHolderTemplate<DERIVED>(*m_pointerToValue->template as<DERIVED>());
}
