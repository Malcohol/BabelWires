/**
 * An EditableValue is a value that can be edited directly within BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Serialization/serializable.hpp>

namespace babelwires {

    /// An editable value can be created within a BabelWires application.
    /// This is in contrast with values that can only be loaded, manipulated using processors and saved.
    /// Since EditableValues can be created directly, BabelWires must be able to serialize them.
    class EditableValue : public Value, public Serializable, public ProjectVisitable {
      public:
        SERIALIZABLE_ABSTRACT(EditableValue, void);
        CLONEABLE_ABSTRACT(EditableValue);

        /// The methods of the ProjectVisitable interface are non-const which is in tension with the fact that
        /// values can be shared. This query allows values to be skipped by the visit when it's known that there is
        /// nothing to do.
        virtual bool canContainIdentifiers() const = 0;

        /// The methods of the ProjectVisitable interface are non-const which is in tension with the fact that
        /// values can be shared. This query allows values to be skipped by the visit when it's known that there is
        /// nothing to do.
        virtual bool canContainFilePaths() const = 0;
    };

} // namespace babelwires
