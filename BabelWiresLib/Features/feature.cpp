/**
 * A Feature is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/feature.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/childValueFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

#include <map>

babelwires::Feature::Feature(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

babelwires::Feature::~Feature() = default;

void babelwires::Feature::setOwner(Feature* owner) {
    m_owner = owner;
}

const babelwires::Feature* babelwires::Feature::getOwner() const {
    return m_owner;
}

babelwires::Feature* babelwires::Feature::getOwnerNonConst() {
    return m_owner;
}

void babelwires::Feature::setChanged(Changes changes) {
    assert((changes != Changes::NothingChanged) && "You cannot call setChanged with no changes to set");
    assert(((changes & ~Changes::SomethingChanged) == Changes::NothingChanged) && "Not a supported change");
    if ((m_changes | changes) != m_changes) {
        m_changes = m_changes | changes;
        if (m_owner) {
            m_owner->setChanged(changes);
        }
    }
}

bool babelwires::Feature::isChanged(Changes changes) const {
    return (m_changes & changes) != Changes::NothingChanged;
}

void babelwires::Feature::clearChanges() {
    m_changes = Changes::NothingChanged;
    for (auto&& child : subfeatures(*this)) {
        child->clearChanges();
    }
}

void babelwires::Feature::setToDefault() {
    doSetToDefault();
}

std::size_t babelwires::Feature::getHash() const {
    return hash::mixtureOf(m_typeRef, *doGetValue());
}

namespace {
    void checkIndex(const babelwires::Feature* f, int i) {
        if ((i < 0) || (i >= f->getNumFeatures())) {
            throw babelwires::ModelException()
                << "Compound feature with " << f->getNumFeatures() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::Feature* babelwires::Feature::getFeature(int i) {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

const babelwires::Feature* babelwires::Feature::getFeature(int i) const {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

void babelwires::Feature::setSubfeaturesToDefault() {
    for (auto&& child : subfeatures(*this)) {
        child->setToDefault();
    }
}

namespace {

    template <typename COMPOUND>
    typename babelwires::CopyConst<COMPOUND, babelwires::Feature>::type*
    tryGetChildFromStepT(COMPOUND* compound, const babelwires::PathStep& step) {
        const int childIndex = compound->getChildIndexFromStep(step);
        if (childIndex >= 0) {
            return compound->getFeature(childIndex);
        }
        return nullptr;
    }

} // namespace

babelwires::Feature* babelwires::Feature::tryGetChildFromStep(const PathStep& step) {
    return tryGetChildFromStepT(this, step);
}

const babelwires::Feature* babelwires::Feature::tryGetChildFromStep(const PathStep& step) const {
    return tryGetChildFromStepT(this, step);
}

babelwires::Feature& babelwires::Feature::getChildFromStep(const PathStep& step) {
    if (Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::Feature& babelwires::Feature::getChildFromStep(const PathStep& step) const {
    if (const Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::TypeRef& babelwires::Feature::getTypeRef() const {
    return m_typeRef;
}

const babelwires::ValueHolder& babelwires::Feature::getValue() const {
    return doGetValue();
}

void babelwires::Feature::setValue(const ValueHolder& newValue) {
    doSetValue(newValue);
}

void babelwires::Feature::assign(const Feature& other) {
    if (getKind() != other.getKind()) {
        throw ModelException() << "Assigning an incompatible value";
    }
    setValue(other.getValue());
}

std::string babelwires::Feature::getKind() const {
    return getType().getKind();
}

const babelwires::TypeSystem& babelwires::Feature::getTypeSystem() const {
    const Feature* current = this;
    while (1) {
        // TODO Query owner first and do a checking downcast when at root.
        if (const SimpleValueFeature* currentAsRootValueFeature = current->as<SimpleValueFeature>()) {
            return currentAsRootValueFeature->getTypeSystem();
        }
        const Feature* const owner = current->getOwner();
        assert(owner && "You can only get the RootValueFeature from a Feature in a hierarchy.");
        current = owner;
    }
}

const babelwires::Type& babelwires::Feature::getType() const {
    const TypeSystem& typeSystem = getTypeSystem();
    return m_typeRef.resolve(typeSystem);
}

int babelwires::Feature::getNumFeatures() const {
    return m_children.size();
}

babelwires::PathStep babelwires::Feature::getStepToChild(const Feature* child) const {
    for (const auto& it : m_children) {
        if (it.getValue().get() == child) {
            return it.getKey0();
        }
    }
    throw ModelException() << "Child not found in owner";
}

int babelwires::Feature::getChildIndexFromStep(const PathStep& step) const {
    const auto it = m_children.find0(step);
    if (it != m_children.end()) {
        step.copyDiscriminatorFrom(it.getKey0());
        return it.getKey1();
    }
    return -1;
}

void babelwires::Feature::synchronizeSubfeatures() {
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

void babelwires::Feature::reconcileChanges(const ValueHolder& other) {
    const ValueHolder& value = getValue();
    if (auto* compound = getType().as<CompoundType>()) {
        // Should only be here if the type hasn't changed, so we can use compound with other.

        std::map<PathStep, Feature*> currentChildFeatures;
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
        if (!isChanged(Changes::SomethingChanged)) {
            if (compound->areDifferentNonRecursively(value, other)) {
                setChanged(Changes::ValueChanged);
            }
        }
    } else if (getValue() != other) {
        setChanged(Changes::ValueChanged);
    }
}
