/**
 * An EditableValue is a value that can be edited directly within BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

const babelwires::EditableValue* babelwires::AlwaysEditableValue::tryGetAsEditableValue() const {
    return this;
}
