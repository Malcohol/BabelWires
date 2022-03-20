/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include "Common/Registry/registry.hpp"
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Type;

    class TypeSystem : public Registry<Type> {
      public:
        TypeSystem();
    };

} // namespace babelwires
