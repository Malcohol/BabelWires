/**
 * A Value is an abstract class for objects which carry a single, usually simple value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <BabelWiresLib/TypeSystem/editableValue.hpp>

const babelwires::EditableValue* babelwires::Value::asEditableValue() const {
    return as<EditableValue>();
}
