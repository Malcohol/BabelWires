#pragma once

#include <BabelWiresLib/Project/projectData.hpp>

namespace testUtils {
    /// Two TestComplexRecordTypes wired together at a subrecord.
    struct TestProjectDataWithCompoundConnection : babelwires::ProjectData {
        TestProjectDataWithCompoundConnection();

        babelwires::NodeId m_sourceNodeId = 4;
        babelwires::NodeId m_targetNodeId = 9;
    };
}
