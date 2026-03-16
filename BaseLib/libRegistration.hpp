/**
 * Registration of factories etc for BaseLib.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    struct DataContext;

    // This is just needed for disambiguation with BabelWiresLib::registerLib.
    namespace baseLib {
        void registerLib(DataContext& context);
    }
} // namespace babelwires
