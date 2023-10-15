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

#include <optional>

namespace babelwires {

    class CompoundType : public Type {
      public:
        virtual int getNumChildren(const Value& compoundValue) const = 0;
        virtual const TypeRef& getChildType(const Value& compoundValue, unsigned int i) const = 0;
        
        virtual std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const Value& compoundValue, unsigned int i) const = 0;
        virtual std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(Value& compoundValue, unsigned int i) const = 0;
        virtual PathStep getStepToChild(const Value& compoundValue, unsigned int i) const = 0;

        virtual std::optional<std::tuple<TypeRef, ValueHolder&>> tryGetChildFromStepNonConst(ValueHolder& compoundValue, const PathStep& step) const = 0;
        virtual std::optional<std::tuple<TypeRef, const ValueHolder&>> tryGetChildFromStep(const Value& compoundValue, const PathStep& step) const = 0;
    };
}
