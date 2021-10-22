/**
 * An interface for objects which associate values and human-friendly names.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <tuple>

namespace babelwires {

    /// An interface for objects which associate values and human-friendly names.
    // TODO Generalize this so value types other than integers can have special values (also, consider using for enums).
    class ValueNames {
      public:
        bool getValueForName(const std::string& name, int& valueOut) const;
        bool getNameForValue(int value, std::string& nameOut) const;

        struct NamedValueIterator {
            void operator++();
            bool operator==(const NamedValueIterator& other) const;
            bool operator!=(const NamedValueIterator& other) const;
            std::tuple<int, std::string> operator*() const;

            int m_value;
            bool m_atEnd;
            const ValueNames& m_valueNamesContainer;
        };

        NamedValueIterator begin() const;
        NamedValueIterator end() const;

      protected:
        virtual int getFirstValue() const = 0;
        virtual bool getNextValueWithName(int& value) const = 0;
        virtual bool doGetNameForValue(int value, std::string& nameOut) const = 0;
        // This method isn't needed for array entries.
        virtual bool doGetValueForName(const std::string& name, int& valueOut) const = 0;
    };

} // namespace babelwires
