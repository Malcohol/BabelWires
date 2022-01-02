/**
 * A ComplexValueEditorData carries enough data to reconstruct a ComplexValueEditor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

namespace babelwires {
    /// Data sufficient to describe the value the editor should be editing.
    struct ComplexValueEditorData {
        ElementId m_elementId;
        FeaturePath m_pathToValue;

        std::size_t getHash() const;

        inline friend bool operator==(const ComplexValueEditorData& a, const ComplexValueEditorData& b) { 
            return (a.m_elementId == b.m_elementId) && (a.m_pathToValue == b.m_pathToValue);
        }

        friend std::ostream& operator<<(std::ostream& os, const ComplexValueEditorData& data) {
            return os << data.m_elementId << ": " << data.m_pathToValue;
        }
    };
}
