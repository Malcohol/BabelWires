#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SimpleValueFeature::SimpleValueFeature(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

std::string babelwires::SimpleValueFeature::doGetValueType() const {
    return "TODO";
}

void babelwires::SimpleValueFeature::doAssign(const ValueFeature& other) {
    if (const auto* otherValueFeature = other.as<SimpleValueFeature>()) {
        const Value& otherValue = otherValueFeature->get();
        if (otherValue != *m_value) {
            const Type& type = getType();
            if (otherValue.isValid(type)) {
                m_value = otherValue.clone();
                setChanged(Changes::ValueChanged);
            } else {
                throw ModelException() << "The new value is not a valid instance of " << m_typeRef.toString();
            }
        }
    } else {
        throw ModelException() << "Cannot assign other kinds of Feature to SimpleValueFeature";
    }
}

void babelwires::SimpleValueFeature::set(const Value& value) {
    if (value != *m_value) {
        const Type& type = getType();
        if (value.isValid(type)) {
            m_value = value.clone();
            setChanged(Changes::ValueChanged);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << m_typeRef.toString();
        }
    }
}

void babelwires::SimpleValueFeature::doSetToDefault() {
    auto newValue = getType().createValue();
    if (*newValue != *m_value) {
        m_value = std::move(newValue);
        setChanged(Changes::ValueChanged);
    }
}

const babelwires::Type& babelwires::SimpleValueFeature::getType() const {
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    return m_typeRef.resolve(context.m_typeSystem);
}

const babelwires::Value& babelwires::SimpleValueFeature::get() const {
    assert(m_value && "The NewValueFeature has not been initialized");
    return *m_value;
}

std::size_t babelwires::SimpleValueFeature::doGetHash() const {
    return hash::mixtureOf(m_typeRef, *m_value);
}
