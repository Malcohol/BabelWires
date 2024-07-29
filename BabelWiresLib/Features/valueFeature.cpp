/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeature.hpp>

#include <BabelWiresLib/Features/childValueFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

#include <map>

babelwires::ValueFeature::ValueFeature(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

babelwires::ValueFeature::~ValueFeature() = default;

const babelwires::TypeRef& babelwires::ValueFeature::getTypeRef() const {
    return m_typeRef;
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
    const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
    return m_typeRef.resolve(typeSystem);
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

int babelwires::ValueFeature::getChildIndexFromStep(const PathStep& step) const {
    const auto it = m_children.find0(step);
    if (it != m_children.end()) {
        return it.getKey1();
    }
    return -1;
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

void babelwires::ValueFeature::synchronizeSubfeatures() {
    const ValueHolder& value = getValue();
    auto* compound = getType().as<CompoundType>();
    if (!compound) {
        return;
    }

    ChildMap newChildMap;
    const unsigned int numChildrenNow = compound->getNumChildren(value);
    for (unsigned int i = 0; i < numChildrenNow; ++i) {
        auto [childValue, step, type] = compound->getChild(value, i);
        std::unique_ptr<ChildValueFeature> childFeature;
        auto it = m_children.find0(step);
        if (it != m_children.end()) {
            childFeature = std::move(it.getValue());
            childFeature->ensureSynchronized(childValue);
        } else {
            childFeature = std::make_unique<ChildValueFeature>(type, childValue);
            childFeature->setOwner(this);
            childFeature->synchronizeSubfeatures();
        }
        newChildMap.insert_or_assign(step, i, std::move(childFeature));
    }
    m_children.swap(newChildMap);
}

babelwires::ValueFeature::RootAndPath<const babelwires::SimpleValueFeature>
babelwires::ValueFeature::getRootValueFeature() const {
    const ValueFeature* current = this;
    std::vector<PathStep> reversePath;
    while (1) {
        if (const SimpleValueFeature* currentAsRootValueFeature = current->as<SimpleValueFeature>()) {
            std::reverse(reversePath.begin(), reversePath.end());
            return {*currentAsRootValueFeature, FeaturePath(std::move(reversePath))};
        }
        assert(getOwner() && "You can only get the RootValueFeature from a ValueFeature in a hierarchy.");
        const ValueFeature* const owner = getOwner()->as<ValueFeature>();
        assert(owner && "The owner of a ChildValueFeature must be a ValueFeature");
        PathStep stepToThis = owner->getStepToChild(current);
        reversePath.emplace_back(stepToThis);
        current = owner;
    }
}

babelwires::ValueFeature::RootAndPath<babelwires::SimpleValueFeature> babelwires::ValueFeature::getRootValueFeature() {
    ValueFeature* current = this;
    std::vector<PathStep> reversePath;
    while (1) {
        if (SimpleValueFeature* currentAsRootValueFeature = current->as<SimpleValueFeature>()) {
            std::reverse(reversePath.begin(), reversePath.end());
            return {*currentAsRootValueFeature, FeaturePath(std::move(reversePath))};
        }
        assert(getOwner() && "You can only get the RootValueFeature from a ValueFeature in a hierarchy.");
        ValueFeature* const owner = current->getOwnerNonConst()->as<ValueFeature>();
        assert(owner && "The owner of a ChildValueFeature must be a ValueFeature");
        PathStep stepToThis = owner->getStepToChild(current);
        reversePath.emplace_back(stepToThis);
        current = owner;
    }
}

void babelwires::ValueFeature::reconcileChanges(const ValueHolder& other) {
    const ValueHolder& value = getValue();
    if (auto* compound = getType().as<CompoundType>()) {
        // Should only be here if the type hasn't changed, so we can use compound with other.

        std::map<PathStep, ValueFeature*> currentChildFeatures;
        for (const auto& it : m_children) {
            currentChildFeatures.emplace(std::pair{it.getKey0(), it.getValue().get()});
        }
        
        std::map<PathStep, const ValueHolder*> backupChildValues;
        for (int i = 0; i < compound->getNumChildren(other); ++i) {
            auto [child, step, _] = compound->getChild(other, i);
            backupChildValues.emplace(std::pair{step, child});
        }

        auto currentIt = currentChildFeatures.begin();
        auto backupIt = backupChildValues.begin();

        while ((currentIt != currentChildFeatures.end()) && (backupIt != backupChildValues.end())) {
            if (currentIt->first < backupIt->first) {
                setChanged(Changes::StructureChanged);                
                ++currentIt;
            } else if (backupIt->first < currentIt->first) {
                setChanged(Changes::StructureChanged);                
                ++backupIt;
            } else {
                // TODO Assert types are the same.
                currentIt->second->reconcileChanges(*backupIt->second);
                ++currentIt;
                ++backupIt;
            }
        }
        if ((currentIt != currentChildFeatures.end()) || (backupIt != backupChildValues.end())) {
            setChanged(Changes::StructureChanged);                
        }
    } else if (getValue() != other) {
        setChanged(Changes::ValueChanged);
    }
}
