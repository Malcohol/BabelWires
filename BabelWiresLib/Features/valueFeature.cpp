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
#include <BabelWiresLib/Features/childValueFeature.hpp>

#include <map>

babelwires::ValueFeature::ValueFeature(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

const babelwires::TypeRef& babelwires::ValueFeature::getTypeRef() const {
    return m_typeRef;
}

const babelwires::ValueHolder& babelwires::ValueFeature::getValue() const {
    return doGetValue();
}

babelwires::ValueHolder& babelwires::ValueFeature::getValue() {
    return doGetValue();
}

void babelwires::ValueFeature::setValue(const ValueHolder& newValue) {
    ValueHolder& currentValue = doGetValue();
    if (!currentValue || ((currentValue != newValue) && (*currentValue != *newValue))) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            currentValue = newValue;
            setChanged(Changes::ValueChanged);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
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
    return m_typeRef.resolve(context.m_typeSystem);
}

int babelwires::ValueFeature::getNumFeatures() const {
    return m_children.size();
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

std::size_t babelwires::ValueFeature::doGetHash() const {
    return hash::mixtureOf(m_typeRef, *doGetValue());
}

void babelwires::ValueFeature::doSetToDefault() {
    assert(m_typeRef && "The type must be set to something non-trivial before doSetToDefault is called");
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    auto [newValue, _] = getType().createValue(context.m_typeSystem);
    ValueHolder& currentValue = doGetValue();
    if (!currentValue || (*newValue != *currentValue)) {
        currentValue = std::move(newValue);
        setChanged(Changes::ValueChanged);
    }
}

void babelwires::ValueFeature::synchronizeSubfeatures() {
    const Value& value = *getValue();

    if (auto* compound = getType().as<CompoundType>()) {

        std::map<ValueHolder*, unsigned int> childValuesNow;

        const unsigned int numChildrenNow = compound->getNumChildren(value);
        for (unsigned int i = 0; i < numChildrenNow; ++i) {
            childValuesNow.emplace(std::pair{compound->getChildNonConst(value, i), i});
        }

        std::map<ValueHolder*, PathStep> currentChildFeatures;
        for (const auto& it : m_children) {
            currentChildFeatures.emplace(std::pair{&it.getValue()->getValue(), it.getKey0()});
        }

        std::vector<std::pair<ValueHolder*, unsigned int>> toAdd;

        auto itValue = childValuesNow.begin();
        auto itFeature = currentChildFeatures.begin();
        while ((itValue != childValuesNow.end()) && (itFeature != currentChildFeatures.end())) {
            if (itValue->first < itFeature->first) {
                toAdd.emplace_back(*itValue);
                ++itValue;
            } else if (itValue->first > itFeature->first) {
                m_children.erase0(itFeature->second);
                ++itFeature;
            } else {
                m_children.find0(itFeature->second).getValue()->synchronizeSubfeatures();
                ++itValue;
                ++itFeature;
            }
        }
        while (itValue != childValuesNow.end()) {
            toAdd.emplace_back(*itValue);
            ++itValue;
        }
        while (itFeature != currentChildFeatures.end()) {
            m_children.erase0(itFeature->second);
            ++itFeature;
        }

        for (auto it : toAdd) {
            ValueHolder* child = compound->getChildNonConst(value, it.second);
            m_children.insert_or_assign(compound->getStepToChild(value, child), it.second,
                std::make_unique<ChildValueFeature>(compound->getChildType(value, it.second), *child));
        }
    }
}