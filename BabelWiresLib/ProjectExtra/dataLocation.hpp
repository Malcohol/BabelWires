/**
 * A DataLocation identifies some data within the system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>

namespace babelwires {
    /// A DataLocation identifies some data within the system.
    class DataLocation : public Cloneable, Serializable, ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(DataLocation);
        CLONEABLE_ABSTRACT(DataLocation);

        // Non-virtual methods which give identity to the data just in terms of elementId and pathToValue.
        virtual std::size_t getHash() const = 0;

        virtual bool operator==(const DataLocation& other) const = 0;
        
        virtual std::string toString() const = 0;
    };
}

namespace std {
    template <> struct hash<babelwires::DataLocation> {
        inline std::size_t operator()(const babelwires::DataLocation& data) const { return data.getHash(); }
    };
} // namespace std
