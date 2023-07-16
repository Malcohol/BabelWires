/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

const babelwires::TypeRef& babelwires::ValueFeature::getTypeRef() const {
    return doGetTypeRef();
}

const babelwires::ValueHolder& babelwires::ValueFeature::getValue() const {
    return doGetValue();
}

void babelwires::ValueFeature::setValue(const ValueHolder& newValue) {
    doSetValue(newValue);
}

void babelwires::ValueFeature::assign(const ValueFeature& other) {
    if (getKind() != other.getKind()) {
        throw ModelException() << "Assigning an incompatible value";
    }
    setValue(other.getValue());
}

std::string babelwires::ValueFeature::getKind() const {
    return getType().getKind();
}

void babelwires::ValueFeature::doSetToDefaultNonRecursive() {
    setToDefault();
}

const babelwires::Type& babelwires::ValueFeature::getType() const {
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    return getTypeRef().resolve(context.m_typeSystem);
}

int babelwires::ValueFeature::getNumFeatures() const {
    if (const CompoundType* const compoundType = getType().as<CompoundType>()) {
        return compoundType->getNumChildren(*getValue());
    }
    return 0;
}

babelwires::PathStep babelwires::ValueFeature::getStepToChild(const Feature* child) const {
    for (const auto& it : m_children) {
        if (it.getValue().get() == child) {
            return it.getKey0();
        }
    }
    throw ModelException() << "Child not found in owner";
}

babelwires::Feature* babelwires::ValueFeature::tryGetChildFromStep(const PathStep& step) {
    const auto it = m_children.find0(step);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

const babelwires::Feature* babelwires::ValueFeature::tryGetChildFromStep(const PathStep& step) const {
    const auto it = m_children.find0(step);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

babelwires::Feature* babelwires::ValueFeature::doGetFeature(int i) {
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

const babelwires::Feature* babelwires::ValueFeature::doGetFeature(int i) const {
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}