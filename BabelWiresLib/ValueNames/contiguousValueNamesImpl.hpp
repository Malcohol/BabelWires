/**
 * Implementations for ValueNames.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/ValueNames/valueNames.hpp"

#include <map>
#include <unordered_map>
#include <vector>

namespace babelwires {

    class ContiguousValueNamesImpl : public ValueNames {
      public:
        typedef std::vector<std::string> Names;
        typedef std::unordered_map<std::string, int> ValueFromNamesMap;

        /// Offset is subtracted from values to give indices.
        ContiguousValueNamesImpl(Names names, int offset = 0);

      protected:
        virtual int getFirstValue() const override;
        virtual bool getNextValueWithName(int& value) const override;
        virtual bool doGetValueForName(const std::string& name, int& valueOut) const override;
        virtual bool doGetNameForValue(int value, std::string& nameOut) const override;

      private:
        const Names m_names;
        const ValueFromNamesMap m_valueFromNames;
        int m_offset;
    };

} // namespace babelwires
