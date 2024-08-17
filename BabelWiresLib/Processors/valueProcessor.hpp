/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Processors/processor.hpp>

#include <memory>

namespace babelwires {
    struct UserLogger;
    class TypeRef;
    class ProjectContext;
    class ValueFeature;
    class SimpleValueFeature;

    class ValueProcessor : public Processor {
      public:
        ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef, const TypeRef& outputTypeRef);
    };
} // namespace babelwires
