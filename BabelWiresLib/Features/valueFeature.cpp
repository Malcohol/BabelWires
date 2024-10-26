/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeature.hpp>

babelwires::ValueFeature::ValueFeature(TypeRef typeRef)
    : Feature(std::move(typeRef)) {}

