#include <BabelWiresLib/babelWiresLibExport.hpp>
/**
 * Registration of factories etc for the main BabelWiresLib
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    class Context;

    /// Registration factories etc. for Standard MIDI File.
    /// Note: This is not a true plugin model, because everything is statically linked.
    BABELWIRESLIB_API void registerLib(babelwires::Context& context);
} // namespace smf
