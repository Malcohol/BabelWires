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

#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>

#include <QString>

#include <cassert>

const babelwires::FileFeature& babelwires::FileRowModel::getFileFeature() const {
    assert(getInputThenOutputFeature()->as<const FileFeature>() && "Wrong type of feature stored");
    return *static_cast<const FileFeature*>(getInputThenOutputFeature());
}

const babelwires::FileElement& babelwires::FileRowModel::getFileElement() const {
    assert(m_featureElement->as<FileElement>() && "A file feature should only appear in a file element");
    return *static_cast<const FileElement*>(m_featureElement);
}

QVariant babelwires::FileRowModel::getValueDisplayData() const {
    return QString(getFileElement().getFilePath().filename().u8string().c_str());
}

QVariant babelwires::FileRowModel::getTooltip() const {
    if (m_featureElement->isFailed()) {
        return RowModel::getTooltip();
    } else {
        // Full path.
        return QString(getFileElement().getFilePath().u8string().c_str());
    }
}

void babelwires::FileRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    const FileElement& fileElement = getFileElement();
    if (isNonzero(fileElement.getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        actionsOut.emplace_back(std::make_unique<ReloadFileAction>());
        actionsOut.emplace_back(std::make_unique<ChangeSourceFileAction>());
    }
    if (isNonzero(fileElement.getSupportedFileOperations() & FileElement::FileOperations::save)) {
        {
            auto saveFileAction = std::make_unique<SaveFileAction>();
            saveFileAction->setEnabled(!getFileElement().getFilePath().empty());
            actionsOut.emplace_back(std::move(saveFileAction));
        }
        actionsOut.emplace_back(std::make_unique<SaveFileAsAction>());
    }
}
