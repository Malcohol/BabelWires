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
        virtual Value* getChild(const Value& compoundValue, int i) = 0;
        virtual const Value* getChild(const Value& compoundValue, int i) const = 0;
        // This may not make sense. Values do not have identity.
        virtual PathStep getStepToChild(const Value& compoundValue, const Value& child) const = 0;
        virtual Value* tryGetChildFromStep(const Value& compoundValue, const PathStep& step) = 0;
        virtual const Value* tryGetChildFromStep(const Value& compoundValue, const PathStep& step) const = 0;
    };
}
