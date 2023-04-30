#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SimpleValueFeature::SimpleValueFeature(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

std::string babelwires::SimpleValueFeature::doGetValueType() const {
    return getType().getKind();
}

void babelwires::SimpleValueFeature::doAssign(const ValueFeature& other) {
    if (const auto* otherValueFeature = other.as<SimpleValueFeature>()) {
        setValueHolder(otherValueFeature->m_value);
    } else {
        throw ModelException() << "Cannot assign other kinds of Feature to SimpleValueFeature";
    }
}

void babelwires::SimpleValueFeature::setValueHolder(const ValueHolder& newValue) {
    if (!m_value || ((m_value != newValue) && (*m_value != *newValue))) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            m_value = newValue;
            setChanged(Changes::ValueChanged);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << m_typeRef.toString();
        }
    }
}

void babelwires::SimpleValueFeature::setValue(const Value& value) {
    if (!m_value || (value != *m_value)) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const Type& type = getType();
        if (type.isValidValue(typeSystem, value)) {
            m_value = value;
            setChanged(Changes::ValueChanged);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << m_typeRef.toString();
        }
    }
}

void babelwires::SimpleValueFeature::setValue(Value&& value) {
    if (!m_value || (value != *m_value)) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const Type& type = getType();
        if (type.isValidValue(typeSystem, value)) {
            m_value = std::move(value);
            setChanged(Changes::ValueChanged);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << m_typeRef.toString();
        }
    }
}

void babelwires::SimpleValueFeature::doSetToDefault() {
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    auto [newValue, _] = getType().createValue(context.m_typeSystem);
    if (!m_value || (*newValue != *m_value)) {
        m_value = std::move(newValue);
        setChanged(Changes::ValueChanged);
    }
}

const babelwires::Type& babelwires::SimpleValueFeature::getType() const {
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    return m_typeRef.resolve(context.m_typeSystem);
}

const babelwires::Value& babelwires::SimpleValueFeature::getValue() const {
    assert(m_value && "The SimpleValueFeature has not been initialized");
    return *m_value;
}

std::size_t babelwires::SimpleValueFeature::doGetHash() const {
    return hash::mixtureOf(m_typeRef, *m_value);
}
