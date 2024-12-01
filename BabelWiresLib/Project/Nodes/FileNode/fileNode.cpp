/**
 * FileNode are Nodes which corresponds to a file (e.g. a source or target file).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>

#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

#include <cassert>

babelwires::FileNode::FileNode(const NodeData& data, NodeId newId)
    : Node(data, newId) {}

bool babelwires::FileNode::reload(const ProjectContext& context, UserLogger& userLogger) {
    assert(!"Reload is not supported on this type of FileNode");
    return false;
}

bool babelwires::FileNode::save(const ProjectContext& context, UserLogger& userLogger) {
    assert(!"Save is not supported on this type of FileNode");
    return false;
}
