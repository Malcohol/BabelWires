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
    class Type;

    // TODO: Could this be unified with ValueFeature, or could they share code?
    class Value : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Value);
        CLONEABLE_ABSTRACT(Value);
        SERIALIZABLE_ABSTRACT(Value, void);

        /// 
        virtual bool isValid(const Type& type) const = 0;
        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const Value& other) const = 0;
    };
} // namespace babelwires
