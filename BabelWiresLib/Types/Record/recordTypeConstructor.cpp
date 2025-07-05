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

std::unique_ptr<babelwires::Type>
babelwires::RecordTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                 const std::vector<const Type*>& typeArguments,
                                                 const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != valueArguments.size()) {
        throw TypeSystemException() << "RecordTypeConstructor requires the same number of types and values, but got "
                                    << typeArguments.size() << " and " << valueArguments.size() << " respectively";
    }

    std::vector<RecordType::Field> fields;
    for (unsigned int i = 0; i < valueArguments.size(); ++i) {
        if (const FieldIdValue* fieldId = valueArguments[i]->as<FieldIdValue>()) {
            fields.emplace_back(RecordType::Field{fieldId->get(), typeArguments[i]->getTypeRef()});
        } else {
            throw TypeSystemException() << "RecordTypeConstructor value argument " << i << " was not a FieldIdValue";
        }
    }

    return std::make_unique<ConstructedType<RecordType>>(std::move(newTypeRef), std::move(fields));
}
