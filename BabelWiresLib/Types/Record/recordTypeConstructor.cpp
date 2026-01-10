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

babelwires::TypePtr
babelwires::RecordTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                                 const TypeConstructorArguments& arguments,
                                                 const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != arguments.getValueArguments().size()) {
        throw TypeSystemException() << "RecordTypeConstructor requires the same number of types and values, but got "
                                    << arguments.getTypeArguments().size() << " and "
                                    << arguments.getValueArguments().size() << " respectively";
    }

    std::vector<RecordType::Field> fields;
    for (unsigned int i = 0; i < arguments.getValueArguments().size(); ++i) {
        if (const FieldIdValue* fieldId = arguments.getValueArguments()[i]->as<FieldIdValue>()) {
            fields.emplace_back(
                RecordType::Field{fieldId->get(), resolvedTypeArguments[i], fieldId->getOptionality()});
        } else {
            throw TypeSystemException() << "RecordTypeConstructor value argument " << i << " was not a FieldIdValue";
        }
    }

    return makeType<RecordType>(std::move(newTypeExp), std::move(fields));
}
