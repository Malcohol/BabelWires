/**
 * The row model for a row corresponding to a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/fileRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/changeSourceFileAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/reloadFileAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAsAction.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>

#include <QString>

#include <cassert>

const babelwires::FileNode& babelwires::FileRowModel::getFileElement() const {
    assert(m_node->tryAs<FileNode>() && "A FileType should only appear in a FileNode");
    return *static_cast<const FileNode*>(m_node);
}

QVariant babelwires::FileRowModel::getValueDisplayData() const {
    return QString(babelwires::pathToString(getFileElement().getFilePath().filename()).c_str());
}

QString babelwires::FileRowModel::getTooltip(ColumnType c) const {
    if (m_node->isFailed()) {
        return RowModel::getTooltip(c);
    } else if (c == ColumnType::Value) {
        // Full path.
        return QString(babelwires::pathToString(getFileElement().getFilePath()).c_str());
    } else {
        return {};
    }
}

void babelwires::FileRowModel::getContextMenuActions(
    std::vector<ContextMenuEntry>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    const FileNode& fileElement = getFileElement();
    if (isNonzero(fileElement.getSupportedFileOperations() & FileNode::FileOperations::reload)) {
        actionsOut.emplace_back(std::make_unique<ReloadFileAction>());
        actionsOut.emplace_back(std::make_unique<ChangeSourceFileAction>());
    }
    if (isNonzero(fileElement.getSupportedFileOperations() & FileNode::FileOperations::save)) {
        {
            auto saveFileAction = std::make_unique<SaveFileAction>();
            saveFileAction->setEnabled(!getFileElement().getFilePath().empty());
            actionsOut.emplace_back(std::move(saveFileAction));
        }
        actionsOut.emplace_back(std::make_unique<SaveFileAsAction>());
    }
}
