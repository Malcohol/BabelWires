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
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>

#include <QString>

#include <cassert>

const babelwires::FileNode& babelwires::FileRowModel::getFileElement() const {
    assert(m_featureElement->as<FileNode>() && "A file feature should only appear in a file element");
    return *static_cast<const FileNode*>(m_featureElement);
}

QVariant babelwires::FileRowModel::getValueDisplayData() const {
    return QString(babelwires::pathToString(getFileElement().getFilePath().filename()).c_str());
}

QString babelwires::FileRowModel::getTooltip() const {
    if (m_featureElement->isFailed()) {
        return RowModel::getTooltip();
    } else {
        // Full path.
        return QString(babelwires::pathToString(getFileElement().getFilePath()).c_str());
    }
}

void babelwires::FileRowModel::getContextMenuActions(
    std::vector<FeatureContextMenuEntry>& actionsOut) const {
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
