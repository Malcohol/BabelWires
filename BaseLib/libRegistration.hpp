/**
 * Registration of factories etc for BaseLib.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

namespace babelwires {
    struct Context;

    // This is just needed for disambiguation with BabelWiresLib::registerLib.
    namespace baseLib {
        BASELIB_API void registerLib(Context& context);
    }
} // namespace babelwires
