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

        virtual std::size_t getHash() const = 0;

        inline friend bool operator==(const DataLocation& a, const DataLocation& b) { 
            return a.equals(b);
        }

        inline friend std::ostream& operator<<(std::ostream& os, const DataLocation& data) {
            data.writeToStream(os);
            return os;
        }

      protected:
        virtual bool equals(const DataLocation& other) const = 0;
        
        virtual void writeToStream(std::ostream& os) const = 0;
    };
}

namespace std {
    template <> struct hash<babelwires::DataLocation> {
        inline std::size_t operator()(const babelwires::DataLocation& data) const { return data.getHash(); }
    };
} // namespace std
