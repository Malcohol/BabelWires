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

#include <BaseLib/Result/error.hpp>

struct babelwires::ValueTreeRoot::ComplexConstructorArguments {
    ComplexConstructorArguments(const TypeSystem& typeSystem, TypePtr typePtr)
        : m_typeSystem(typeSystem)
        , m_typePtr(std::move(typePtr)) {
        auto [newValue, _] = m_typePtr->createValue(typeSystem);
        m_value = newValue;
    }

    const TypeSystem& m_typeSystem;
    TypePtr m_typePtr;
    ValueHolder m_value;
};

babelwires::ValueTreeRoot::ValueTreeRoot(ComplexConstructorArguments&& arguments)
    : ValueTreeNode(std::move(arguments.m_typePtr), std::move(arguments.m_value))
    , m_typeSystem(arguments.m_typeSystem) {
        initializeChildren(arguments.m_typeSystem);
    }

babelwires::ValueTreeRoot::ValueTreeRoot(const TypeSystem& typeSystem, TypePtr typePtr)
    : ValueTreeRoot(ComplexConstructorArguments(typeSystem, std::move(typePtr))) {}

babelwires::Result babelwires::ValueTreeRoot::doSetValue(const ValueHolder& newValue) {
    if (getValue() != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = *getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            reconcileChangesAndSynchronizeChildren(m_typeSystem, newValue);
        } else {
            return Error() << "The new value is not a valid instance of " << getType()->getTypeExp().toString();
        }
    }
    return {};
}

void babelwires::ValueTreeRoot::doSetToDefault() {
    const TypeSystem& typeSystem = getTypeSystem();
    const Type& type = *getType();
    auto [newValue, _] = type.createValue(typeSystem);
    if (getValue() != newValue) {
        reconcileChangesAndSynchronizeChildren(m_typeSystem, newValue);
    }
}

void babelwires::ValueTreeRoot::setDescendentValue(const Path& path, const ValueHolder& newValue) {
    ValueHolder newRootValue = getValue();
    auto [_, valueInCopy] = assertFollowPathNonConst(m_typeSystem, *getType(), path, newRootValue);
    valueInCopy = newValue;
    reconcileChangesAndSynchronizeChildren(m_typeSystem, newRootValue, path);
}

const babelwires::TypeSystem& babelwires::ValueTreeRoot::getTypeSystem() const {
    return m_typeSystem;
}
