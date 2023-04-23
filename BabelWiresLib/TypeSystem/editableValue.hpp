/**
 * An EditableValue is a value that can be edited directly within BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Serialization/serializable.hpp>

namespace babelwires {

    /// An editable value can be created within a BabelWires application.
    /// This is in contrast with values that can only be loaded, manipulated using processors and saved.
    /// Since EditableValues can be created directly, BabelWires must be able to serialize them.
    class EditableValue : public Value, public Serializable {
      public:
        SERIALIZABLE_ABSTRACT(EditableValue, void);
        CLONEABLE_ABSTRACT(EditableValue);
    };

} // namespace babelwires
