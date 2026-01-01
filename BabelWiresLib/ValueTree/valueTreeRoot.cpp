/**
 * A ValueTreeRoot is a ValueTreeNode which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

struct babelwires::ValueTreeRoot::ComplexConstructorArguments {
    ComplexConstructorArguments(const TypeSystem& typeSystem, TypeExp typeExp)
        : m_typeSystem(typeSystem)
        , m_typeRef(std::move(typeExp)) {
        // TODO Do we need to handle failure here? Use tryResolve and possibly fall back to FailureType?
        const TypePtr& type = m_typeRef.resolve(typeSystem);
        auto [newValue, _] = type->createValue(typeSystem);
        m_value = newValue;
    }
    const TypeSystem& m_typeSystem;
    TypeExp m_typeRef;
    ValueHolder m_value;
};

babelwires::ValueTreeRoot::ValueTreeRoot(ComplexConstructorArguments&& arguments)
    : ValueTreeNode(std::move(arguments.m_typeRef), std::move(arguments.m_value))
    , m_typeSystem(arguments.m_typeSystem) {
        initializeChildren(arguments.m_typeSystem);
    }

babelwires::ValueTreeRoot::ValueTreeRoot(const TypeSystem& typeSystem, TypeExp typeExp)
    : ValueTreeRoot(ComplexConstructorArguments(typeSystem, std::move(typeExp))) {}

void babelwires::ValueTreeRoot::doSetValue(const ValueHolder& newValue) {
    if (getValue() != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            reconcileChangesAndSynchronizeChildren(m_typeSystem, newValue);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeExp().toString();
        }
    }
}

void babelwires::ValueTreeRoot::doSetToDefault() {
    assert(getTypeExp() && "The type must be set to something non-trivial before doSetToDefault is called");
    const TypeSystem& typeSystem = getTypeSystem();
    const Type& type = getType();
    auto [newValue, _] = type.createValue(typeSystem);
    if (getValue() != newValue) {
        reconcileChangesAndSynchronizeChildren(m_typeSystem, newValue);
    }
}

void babelwires::ValueTreeRoot::setDescendentValue(const Path& path, const ValueHolder& newValue) {
    ValueHolder newRootValue = getValue();
    auto [_, valueInCopy] = followPathNonConst(m_typeSystem, getType(), path, newRootValue);
    valueInCopy = newValue;
    reconcileChangesAndSynchronizeChildren(m_typeSystem, newRootValue, path);
}

const babelwires::TypeSystem& babelwires::ValueTreeRoot::getTypeSystem() const {
    return m_typeSystem;
}
