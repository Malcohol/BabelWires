/**
 * A TypeConstructor which constructs an array type with a particular entry type, size range and default size.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

babelwires::TypeVariableTypeConstructor::VariableData
babelwires::TypeVariableTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    // TODO make optional
    if (valueArguments.size() != 2) {
        throw TypeSystemException() << "TypeVariableTypeConstructor expects 2 value arguments but got "
                                    << valueArguments.size();
    }

    IntValue::NativeType args[2];
    for (int i = 0; i < 2; ++i) {
        if (const IntValue* intValue = valueArguments[i]->as<IntValue>()) {
            const IntValue::NativeType nativeValue = intValue->get();
            if (nativeValue < 0) {
                throw TypeSystemException()
                    << "Value argument " << i << " given to TypeVariableTypeConstructor was negative";
            }
            if (nativeValue > std::numeric_limits<unsigned int>::max()) {
                throw TypeSystemException()
                    << "Value argument " << i << "given to TypeVariableTypeConstructor was too large";
            }
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Value argument " << i
                                        << " given to TypeVariableTypeConstructor was not an IntValue";
        }
    }
    return {static_cast<unsigned int>(args[0]), static_cast<unsigned int>(args[1])};
}

babelwires::TypeConstructor::TypeConstructorResult
babelwires::TypeVariableTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const {
    // Verify the arguments are valid, but actually they don't need to be passed to the resulting types.
    /*VariableData variableData =*/ extractValueArguments(arguments.m_valueArguments);

    assert(resolvedTypeArguments.size() == arguments.m_typeArguments.size());
    if (arguments.m_typeArguments.size() > 1) {
        throw TypeSystemException() << "TypeVariableTypeConstructor expects 0 or 1 type arguments but got "
                                    << arguments.m_typeArguments.size();
    }
    if (arguments.m_typeArguments.size() == 1) {
        return resolvedTypeArguments[0];
    } else {
        return std::make_unique<ConstructedType<TypeVariableType>>(std::move(newTypeRef));
    }
}

babelwires::TypeRef babelwires::TypeVariableTypeConstructor::makeTypeRef(unsigned int typeVariableIndex,
                                                                         unsigned int numGenericTypeLevels) {
    return babelwires::TypeRef{getThisIdentifier(), babelwires::IntValue(typeVariableIndex),
                               babelwires::IntValue(numGenericTypeLevels)};
}

std::optional<babelwires::TypeVariableTypeConstructor::VariableData> babelwires::TypeVariableTypeConstructor::isTypeVariable(const TypeRef& typeRef) {
    struct Visitor {
        std::optional<VariableData> operator()(std::monostate) {
            return {};
        }
        std::optional<VariableData> operator()(const RegisteredTypeId& typeId) { 
            // Reasonable assumption: no one would register a type variable type.
            return {};
        }
        std::optional<VariableData> operator()(const TypeConstructorId& constructorId, const TypeConstructorArguments& constructorArguments) {
            if (constructorId == TypeVariableTypeConstructor::getThisIdentifier()) {
                return extractValueArguments(constructorArguments.m_valueArguments);
            } else {
                return {};
            }
        }
    } visitor;
    return typeRef.visit<Visitor, std::optional<VariableData>>(visitor);
}