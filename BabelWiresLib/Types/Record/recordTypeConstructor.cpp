/**
 * Construct a simple record from types and fieldIds.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

babelwires::TypeConstructor::TypeConstructorResult
babelwires::RecordTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                 const TypeConstructorArguments& arguments,
                                                 const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_typeArguments.size() != arguments.m_valueArguments.size()) {
        throw TypeSystemException() << "RecordTypeConstructor requires the same number of types and values, but got "
                                    << arguments.m_typeArguments.size() << " and " << arguments.m_valueArguments.size()
                                    << " respectively";
    }

    std::vector<RecordType::Field> fields;
    for (unsigned int i = 0; i < arguments.m_valueArguments.size(); ++i) {
        if (const FieldIdValue* fieldId = arguments.m_valueArguments[i]->as<FieldIdValue>()) {
            fields.emplace_back(RecordType::Field{fieldId->get(), arguments.m_typeArguments[i]});
        } else {
            throw TypeSystemException() << "RecordTypeConstructor value argument " << i << " was not a FieldIdValue";
        }
    }

    return std::make_unique<ConstructedType<RecordType>>(std::move(newTypeRef), std::move(fields));
}
