/**
 * Utilities useful for clients of the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    struct UiPosition;
    struct ProjectData;

    namespace projectUtilities {
        void translate(const UiPosition& offset, ProjectData& dataInOut);
    }
} // namespace babelwires