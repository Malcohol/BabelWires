/**
 * A ChildValueFeature is a ValueFeature whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/childValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::ChildValueFeature::ChildValueFeature(TypeRef typeRef, const ValueHolder& valueHolder)
    : ValueFeature(std::move(typeRef))
    , m_value(valueHolder) {}

const babelwires::ValueHolder& babelwires::ChildValueFeature::doGetValue() const {
    return m_value;
}

void babelwires::ChildValueFeature::doSetValue(const ValueHolder& newValue) {
    const ValueHolder& currentValue = doGetValue();
    if (currentValue != newValue) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            auto rootAndPath = getRootValueFeature();
            ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
            modifiableValueHolder = newValue;
            // Changing the modifiableValueHolder, can change the value.
            synchronizeSubfeatures();
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
}

void babelwires::ChildValueFeature::doSetToDefault() {
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    auto [newValue, _] = getType().createValue(context.m_typeSystem);
    auto rootAndPath = getRootValueFeature();
    ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
    modifiableValueHolder = newValue;
    // Changing the modifiableValueHolder can change the structure of the hierarchy at this point.
    synchronizeSubfeatures();
}