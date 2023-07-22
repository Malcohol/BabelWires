/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Features/Path/pathStep.hpp>

namespace babelwires {

    class CompoundType : public Type {
      public:
        virtual int getNumChildren(const Value& compoundValue) const = 0;
        virtual const TypeRef& getChildType(const Value& compoundValue, int i) const = 0;
        virtual ValueHolder* getChildNonConst(const Value& compoundValue, int i) const = 0;
        virtual const ValueHolder* getChild(const Value& compoundValue, int i) const = 0;
        // Hmmm. Giving ValueHolder identity here.
        virtual PathStep getStepToChild(const Value& compoundValue, const ValueHolder* child) const = 0;
        virtual ValueHolder* tryGetChildFromStepNonConst(const Value& compoundValue, const PathStep& step) const = 0;
        virtual const ValueHolder* tryGetChildFromStep(const Value& compoundValue, const PathStep& step) const = 0;
    };
}
