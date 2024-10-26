/**
 * A ValueTreeNode is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueTreeNode.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/childValueFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/valueTreeRoot.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

#include <map>

babelwires::ValueTreeNode::ValueTreeNode(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

babelwires::ValueTreeNode::~ValueTreeNode() = default;

void babelwires::ValueTreeNode::setOwner(ValueTreeNode* owner) {
    m_owner = owner;
}

const babelwires::ValueTreeNode* babelwires::ValueTreeNode::getOwner() const {
    return m_owner;
}

babelwires::ValueTreeNode* babelwires::ValueTreeNode::getOwnerNonConst() {
    return m_owner;
}

void babelwires::ValueTreeNode::setChanged(Changes changes) {
    assert((changes != Changes::NothingChanged) && "You cannot call setChanged with no changes to set");
    assert(((changes & ~Changes::SomethingChanged) == Changes::NothingChanged) && "Not a supported change");
    if ((m_changes | changes) != m_changes) {
        m_changes = m_changes | changes;
        if (m_owner) {
            m_owner->setChanged(changes);
        }
    }
}

bool babelwires::ValueTreeNode::isChanged(Changes changes) const {
    return (m_changes & changes) != Changes::NothingChanged;
}

void babelwires::ValueTreeNode::clearChanges() {
    m_changes = Changes::NothingChanged;
    for (auto&& child : subfeatures(*this)) {
        child->clearChanges();
    }
}

void babelwires::ValueTreeNode::setToDefault() {
    doSetToDefault();
}

std::size_t babelwires::ValueTreeNode::getHash() const {
    return hash::mixtureOf(m_typeRef, *doGetValue());
}

namespace {
    void checkIndex(const babelwires::ValueTreeNode* f, int i) {
        if ((i < 0) || (i >= f->getNumFeatures())) {
            throw babelwires::ModelException()
                << "Compound feature with " << f->getNumFeatures() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::ValueTreeNode* babelwires::ValueTreeNode::getFeature(int i) {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::ValueTreeNode::getFeature(int i) const {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

void babelwires::ValueTreeNode::setSubfeaturesToDefault() {
    for (auto&& child : subfeatures(*this)) {
        child->setToDefault();
    }
}

namespace {

    template <typename COMPOUND>
    typename babelwires::CopyConst<COMPOUND, babelwires::ValueTreeNode>::type*
    tryGetChildFromStepT(COMPOUND* compound, const babelwires::PathStep& step) {
        const int childIndex = compound->getChildIndexFromStep(step);
        if (childIndex >= 0) {
            return compound->getFeature(childIndex);
        }
        return nullptr;
    }

} // namespace

babelwires::ValueTreeNode* babelwires::ValueTreeNode::tryGetChildFromStep(const PathStep& step) {
    return tryGetChildFromStepT(this, step);
}

const babelwires::ValueTreeNode* babelwires::ValueTreeNode::tryGetChildFromStep(const PathStep& step) const {
    return tryGetChildFromStepT(this, step);
}

babelwires::ValueTreeNode& babelwires::ValueTreeNode::getChildFromStep(const PathStep& step) {
    if (ValueTreeNode* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::ValueTreeNode& babelwires::ValueTreeNode::getChildFromStep(const PathStep& step) const {
    if (const ValueTreeNode* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::TypeRef& babelwires::ValueTreeNode::getTypeRef() const {
    return m_typeRef;
}

const babelwires::ValueHolder& babelwires::ValueTreeNode::getValue() const {
    return doGetValue();
}

void babelwires::ValueTreeNode::setValue(const ValueHolder& newValue) {
    doSetValue(newValue);
}

void babelwires::ValueTreeNode::assign(const ValueTreeNode& other) {
    if (getKind() != other.getKind()) {
        throw ModelException() << "Assigning an incompatible value";
    }
    setValue(other.getValue());
}

std::string babelwires::ValueTreeNode::getKind() const {
    return getType().getKind();
}

const babelwires::TypeSystem& babelwires::ValueTreeNode::getTypeSystem() const {
    const ValueTreeNode* current = this;
    while (1) {
        // TODO Query owner first and do a checking downcast when at root.
        if (const ValueTreeRoot* currentAsRootValueFeature = current->as<ValueTreeRoot>()) {
            return currentAsRootValueFeature->getTypeSystem();
        }
        const ValueTreeNode* const owner = current->getOwner();
        assert(owner && "You can only get the RootValueFeature from a ValueTreeNode in a hierarchy.");
        current = owner;
    }
}

const babelwires::Type& babelwires::ValueTreeNode::getType() const {
    const TypeSystem& typeSystem = getTypeSystem();
    return m_typeRef.resolve(typeSystem);
}

int babelwires::ValueTreeNode::getNumFeatures() const {
    return m_children.size();
}

babelwires::PathStep babelwires::ValueTreeNode::getStepToChild(const ValueTreeNode* child) const {
    for (const auto& it : m_children) {
        if (it.getValue().get() == child) {
            return it.getKey0();
        }
    }
    throw ModelException() << "Child not found in owner";
}

int babelwires::ValueTreeNode::getChildIndexFromStep(const PathStep& step) const {
    const auto it = m_children.find0(step);
    if (it != m_children.end()) {
        step.copyDiscriminatorFrom(it.getKey0());
        return it.getKey1();
    }
    return -1;
}

void babelwires::ValueTreeNode::synchronizeSubfeatures() {
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

void babelwires::ValueTreeNode::reconcileChanges(const ValueHolder& other) {
    const ValueHolder& value = getValue();
    if (auto* compound = getType().as<CompoundType>()) {
        // Should only be here if the type hasn't changed, so we can use compound with other.

        std::map<PathStep, ValueTreeNode*> currentChildFeatures;
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
