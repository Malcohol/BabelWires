/**
 * A DataLocation identifies some data within the system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <BaseLib/Cloning/cloneable.hpp>
#include <BaseLib/Serialization/serializable.hpp>

namespace babelwires {
    /// A DataLocation identifies some data within the system.
    /// It's usually the case that values can have have subvalues, so this class carries a path
    /// to allow it to identify those subvalues.
    /// Note: In theory, a path on its own could be used to express the current use-cases, but I do not
    /// want to assume that will always be a possible or convenient option.
    class DataLocation : public Cloneable, public Serializable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(DataLocation);
        CLONEABLE_ABSTRACT(DataLocation);

        DataLocation(Path pathToValue);

        const Path& getPathToValue() const;
        Path& getPathToValue();

        virtual std::size_t getHash() const;

        inline friend bool operator==(const DataLocation& a, const DataLocation& b) { return a.equals(b); }

        inline friend std::ostream& operator<<(std::ostream& os, const DataLocation& data) {
            os << "\"";
            data.writeToStream(os);
            return os << "\"";
        }

      public:
        // Serialization.
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      protected:
        DataLocation() = default;

        virtual bool equals(const DataLocation& other) const;

        virtual void writeToStream(std::ostream& os) const;

      private:
        Path m_pathToValue;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::DataLocation> {
        inline std::size_t operator()(const babelwires::DataLocation& data) const { return data.getHash(); }
    };
} // namespace std
