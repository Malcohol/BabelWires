/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Cloning/cloneable.hpp>

namespace babelwires {
    class TypeSystem;

    class Value : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Value);
        CLONEABLE_ABSTRACT(Value);
        SERIALIZABLE_ABSTRACT(Value, void);

        /// 
        virtual bool isValid(const TypeSystem& typeSystem, LongIdentifier type) const = 0;
        virtual std::size_t getHash() const = 0;
    };
} // namespace babelwires
