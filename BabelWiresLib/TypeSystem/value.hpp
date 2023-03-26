/**
 * A Value is an abstract class for objects which carry a single, usually simple value.
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

        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const Value& other) const = 0;
        virtual std::string toString() const = 0;

        bool operator!= (const Value& other) const { return !(*this == other); }
    };
} // namespace babelwires


namespace std {
    template <> struct hash<babelwires::Value> {
        inline std::size_t operator()(const babelwires::Value& value) const { return value.getHash(); }
    };
} // namespace std
