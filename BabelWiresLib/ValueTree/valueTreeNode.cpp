/**
 * A ValueTreeNode is the base class of nodes in the ValueTree.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeChild.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <map>

babelwires::ValueTreeNode::ValueTreeNode(TypeExp typeExp, ValueHolder value)
    : m_typeRef(std::move(typeExp))
    , m_value(std::move(value)) {}

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
    for (auto&& child : getChildRange(*this)) {
        child->clearChanges();
    }
}

void babelwires::ValueTreeNode::setToDefault() {
    doSetToDefault();
}

std::size_t babelwires::ValueTreeNode::getHash() const {
    return hash::mixtureOf(m_typeRef, m_value);
}

namespace {
    void checkIndex(const babelwires::ValueTreeNode* f, int i) {
        if ((i < 0) || (i >= f->getNumChildren())) {
            throw babelwires::ModelException()
                << "Compound with " << f->getNumChildren() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::ValueTreeNode* babelwires::ValueTreeNode::getChild(int i) {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::ValueTreeNode::getChild(int i) const {
    checkIndex(this, i);
    const auto it = m_children.find1(i);
    if (it != m_children.end()) {
        return it.getValue().get();
    }
    return nullptr;
}

namespace {

    template <typename COMPOUND>
    typename babelwires::CopyConst<COMPOUND, babelwires::ValueTreeNode>::type*
    tryGetChildFromStepT(COMPOUND* compound, const babelwires::PathStep& step) {
        const int childIndex = compound->getChildIndexFromStep(step);
        if (childIndex >= 0) {
            return compound->getChild(childIndex);
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

const babelwires::TypeExp& babelwires::ValueTreeNode::getTypeExp() const {
    return m_typeRef;
}

const babelwires::ValueHolder& babelwires::ValueTreeNode::getValue() const {
    assert(m_value && "The ValueTreeRoot has not been initialized");
    return m_value;
}

void babelwires::ValueTreeNode::setValue(const ValueHolder& newValue) {
    doSetValue(newValue);
}

void babelwires::ValueTreeNode::assign(const ValueTreeNode& other) {
    setValue(other.getValue());
}

std::string babelwires::ValueTreeNode::getFlavour() const {
    return getType().getFlavour();
}

const babelwires::TypeSystem& babelwires::ValueTreeNode::getTypeSystem() const {
    const ValueTreeNode* current = this;
    while (1) {
        // TODO Query owner first and do a checking downcast when at root.
        if (const ValueTreeRoot* currentAsRoot = current->as<ValueTreeRoot>()) {
            return currentAsRoot->getTypeSystem();
        }
        const ValueTreeNode* const owner = current->getOwner();
        assert(owner && "You can only get the ValueTreeRoot from a ValueTreeNode in a hierarchy.");
        current = owner;
    }
}

const babelwires::Type& babelwires::ValueTreeNode::getType() const {
    const TypeSystem& typeSystem = getTypeSystem();
    // TODO This assumes the ValueTreeNode will be changed store TypePtr instead of TypeExp.
    return *m_typeRef.resolve(typeSystem);
}

int babelwires::ValueTreeNode::getNumChildren() const {
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
        // TODO DISCRIMINATORS
        return it.getKey1();
    }
    return -1;
}

void babelwires::ValueTreeNode::initializeChildren(const TypeSystem& typeSystem) {
    // TODO: Do in constructor?
    const ValueHolder& value = getValue();
    auto* compound = getType().as<CompoundType>();
    if (!compound) {
        return;
    }

    const unsigned int numChildrenNow = compound->getNumChildren(value);
    for (unsigned int i = 0; i < numChildrenNow; ++i) {
        auto [childValue, step, childTypeRef] = compound->getChild(value, i);
        auto child = std::make_unique<ValueTreeChild>(childTypeRef, *childValue, this);
        child->initializeChildren(typeSystem);
        m_children.insert_or_assign(step, i, std::move(child));
    }
}

void babelwires::ValueTreeNode::reconcileChangesAndSynchronizeChildren(const TypeSystem& typeSystem,
                                                                       const ValueHolder& other) {
    const ValueHolder& value = getValue();

    Changes changes = Changes::NothingChanged;

    if (auto* compound = getType().as<CompoundType>()) {
        // Should only be here if the type hasn't changed, so we can use compound with other.

        std::map<PathStep, std::unique_ptr<ValueTreeChild>*> currentChildren;
        for (const auto& it : m_children) {
            currentChildren.emplace(std::pair{it.getKey0(), &it.getValue()});
        }

        struct NewChildInfo {
            const ValueHolder* m_value;
            const TypeExp& m_typeRef;
            unsigned int m_index;
        };

        std::map<PathStep, NewChildInfo> otherValues;
        unsigned int newNumChildren = compound->getNumChildren(other);
        for (unsigned int i = 0; i < newNumChildren; ++i) {
            auto [child, step, childTypeRef] = compound->getChild(other, i);
            otherValues.emplace(std::pair{step, NewChildInfo{child, childTypeRef, i}});
        }

        auto currentIt = currentChildren.begin();
        auto otherIt = otherValues.begin();

        ChildMap newChildMap;
        // TODO newChildMap.reserve(newNumChildren);

        auto addNewChild = [this, &typeSystem, &newChildMap](const auto& otherIt) {
            auto child = std::make_unique<ValueTreeChild>(otherIt->second.m_typeRef, *otherIt->second.m_value, this);
            child->initializeChildren(typeSystem);
            newChildMap.insert_or_assign(otherIt->first, otherIt->second.m_index, std::move(child));
        };

        while ((currentIt != currentChildren.end()) && (otherIt != otherValues.end())) {
            if (currentIt->first < otherIt->first) {
                changes = changes | Changes::StructureChanged;
                ++currentIt;
            } else if (otherIt->first < currentIt->first) {
                changes = changes | Changes::StructureChanged;
                addNewChild(otherIt);
                ++otherIt;
            } else {
                // Preserve an existing child.
                std::unique_ptr<ValueTreeChild> temp;
                temp.swap(*currentIt->second);
                // Types may change, e.g. when a type variable is assigned.
                if (temp->m_typeRef != otherIt->second.m_typeRef) {
                    temp->m_typeRef = otherIt->second.m_typeRef;
                    // This ensures the UI updates the connectivity of the node, since a type variable may have been
                    // assigned, allowing connections at compound nodes, or reset, disallowing them.
                    // This is more blunt than it needs to be, but type changes are probably rare.
                    setChanged(Changes::StructureChanged);
                }
                temp->reconcileChangesAndSynchronizeChildren(typeSystem, *otherIt->second.m_value);
                newChildMap.insert_or_assign(otherIt->first, otherIt->second.m_index, std::move(temp));
                ++currentIt;
                ++otherIt;
            }
        }
        if ((currentIt != currentChildren.end()) || (otherIt != otherValues.end())) {
            changes = changes | Changes::StructureChanged;
        }
        while (otherIt != otherValues.end()) {
            addNewChild(otherIt);
            ++otherIt;
        }
        m_children.swap(newChildMap);

        if (compound->areDifferentNonRecursively(value, other)) {
            changes = changes | Changes::ValueChanged;
        }
    } else if (m_value != other) {
        changes = changes | Changes::ValueChanged;
    }
    m_value = other;
    if (changes != Changes::NothingChanged) {
        setChanged(changes);
    }
}

void babelwires::ValueTreeNode::reconcileChangesAndSynchronizeChildren(const TypeSystem& typeSystem,
                                                                       const ValueHolder& other, const Path& path,
                                                                       unsigned int pathIndex) {
    if (pathIndex == path.getNumSteps()) {
        reconcileChangesAndSynchronizeChildren(typeSystem, other);
        return;
    }
    const PathStep step = path.getStep(pathIndex);
    auto childWithChangesIt = m_children.find0(step);

    auto compoundType = getType().as<CompoundType>();
    auto [childValue, step2, childTypeRef] =
        compoundType->getChild(other, compoundType->getChildIndexFromStep(other, step));
    assert(step == step2);

    // TODO: Assert that all values off the path are unchanged.

    m_value = other;
    childWithChangesIt.getValue()->reconcileChangesAndSynchronizeChildren(typeSystem, *childValue, path, pathIndex + 1);
}

void babelwires::ValueTreeNode::reconcileChangesAndSynchronizeChildren(const TypeSystem& typeSystem,
                                                                       const ValueHolder& other, const Path& path) {
    reconcileChangesAndSynchronizeChildren(typeSystem, other, path, 0);
}