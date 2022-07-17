/**
 * Implementations for ValueNames.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueNames/valueNames.hpp>

#include <map>
#include <unordered_map>
#include <vector>

namespace babelwires {

    class SparseValueNamesImpl : public ValueNames {
      public:
        typedef std::map<int, std::string> NameFromValuesMap;
        typedef std::unordered_map<std::string, int> ValueFromNamesMap;

        SparseValueNamesImpl(NameFromValuesMap nameFromValues);

      protected:
        virtual int getFirstValue() const override;
        virtual bool getNextValueWithName(int& value) const override;
        virtual bool doGetValueForName(const std::string& name, int& valueOut) const override;
        virtual bool doGetNameForValue(int value, std::string& nameOut) const override;

      private:
        const NameFromValuesMap m_nameFromValues;
        const ValueFromNamesMap m_valueFromNames;
    };

} // namespace babelwires
