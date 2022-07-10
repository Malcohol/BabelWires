/**
 * Registration of factories etc for the main BabelWiresLib
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    struct ProjectContext;

    /// Registration factories etc. for Standard MIDI File.
    /// Note: This is not a true plugin model, because everything is statically linked.
    void registerLib(babelwires::ProjectContext& context);
} // namespace smf
