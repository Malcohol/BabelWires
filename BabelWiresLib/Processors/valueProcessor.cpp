/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/valueProcessor.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::ValueProcessor::ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                                           const TypeRef& outputTypeRef)
    : Processor(projectContext, inputTypeRef, outputTypeRef) {}
    