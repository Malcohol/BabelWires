/**
 * Base class of models corresponding to a row in a node of the flow graph.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/projectCommandContextMenuAction.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Nodes/contentsCache.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <QBrush>

#include <cassert>

void babelwires::RowModel::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem) {}

bool babelwires::RowModel::hasInput() const {
    return m_contentsCacheEntry->getInput();
}

const babelwires::ValueTreeNode* babelwires::RowModel::getInput() const {
    return m_contentsCacheEntry->getInput();
}

const babelwires::ValueTreeNode* babelwires::RowModel::getOutput() const {
    return m_contentsCacheEntry->getOutput();
}

const babelwires::ValueTreeNode* babelwires::RowModel::getInputThenOutput() const {
    return m_contentsCacheEntry->getInputThenOutput();
}

const babelwires::ValueTreeNode* babelwires::RowModel::getOutputThenInput() const {
    return m_contentsCacheEntry->getOutputThenInput();
}

bool babelwires::RowModel::isModified() const {
    assert(!m_contentsCacheEntry->hasModifier() ||
           hasInput() && "It does not make sense for a modifier to be present when there is no input ValueTreeNode");
    return m_contentsCacheEntry->hasModifier();
}

QVariant babelwires::RowModel::getValueDisplayData() const {
    return {};
}

QString babelwires::RowModel::getTooltip(ColumnType c) const {
    if (m_node->isFailed()) {
        return m_node->getReasonForFailure().c_str();
    } else if (m_contentsCacheEntry->hasFailedModifier()) {
        const Modifier* modifier = m_node->findModifier(m_contentsCacheEntry->getPath());
        assert(modifier && "The cache is inconsistent with the data");
        assert(modifier->isFailed() && "The cache is inconsistent with the data");
        return modifier->getReasonForFailure().c_str();
    } else if (m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        QString message = "There are failed modifiers:";
        int numFailedModifiers = 0;
        for (const Modifier* modifier : m_node->getEdits().modifierRange(m_contentsCacheEntry->getPath())) {
            if (modifier->isFailed()) {
                message += QString("\n\u2022 ") + modifier->getReasonForFailure().c_str();
                ++numFailedModifiers;
                if (numFailedModifiers == 10) {
                    message += QString("\n\u2022 \u2026");
                    break;
                }
            }
        }
        assert(numFailedModifiers && "The cache is inconsistent with the data");
        return message;
    } else {
        return {};
    }
}

babelwires::RowModel::BackgroundStyle babelwires::RowModel::getBackgroundStyle(ColumnType c) const {
    if (m_node->isFailed()) {
        return BackgroundStyle::failed;
    } else if (m_contentsCacheEntry->hasFailedModifier()) {
        return BackgroundStyle::failed;
    } else if (m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        return BackgroundStyle::failedHidden;
    } else if ((c == ColumnType::Value) && hasInput()) {
        if (isItemEditable()) {
            return BackgroundStyle::editable;
        }
    }
    return BackgroundStyle::normal;
}

bool babelwires::RowModel::isItemEditable() const {
    return false;
}

QWidget* babelwires::RowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    return nullptr;
}

void babelwires::RowModel::setEditorData(QWidget* editor) const {}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::RowModel::createCommandFromEditor(QWidget* editor) const {
    return nullptr;
}

bool babelwires::RowModel::hasCustomPainting() const {
    return false;
}

void babelwires::RowModel::paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const {
    assert(false && "hasCustomPainting returned false");
}

QSize babelwires::RowModel::sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const {
    assert(false && "hasCustomPainting returned false");
    return {};
}

void babelwires::RowModel::getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const {
    const NodeId nodeId = m_node->getNodeId();
    if (isModified()) {
        actionsOut.emplace_back(std::make_unique<ProjectCommandContextMenuAction>(
            std::make_unique<RemoveModifierCommand>("Remove modifier", nodeId, m_contentsCacheEntry->getPath())
        ));
    }
    if (m_contentsCacheEntry->hasFailedModifier() || m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        actionsOut.emplace_back(std::make_unique<ProjectCommandContextMenuAction>(
            std::make_unique<RemoveFailedModifiersCommand>("Remove failed modifiers", nodeId, m_contentsCacheEntry->getPath())
        ));
    }
    if (const babelwires::ValueTreeNode* input = getInput()) {
        auto [compound, currentSize, range, initialSize] = ValueTreeHelper::getInfoFromArray(input->getOwner());
        if (compound) {
            const bool arrayActionsAreEnabled = m_contentsCacheEntry->isStructureEditable();
            // QString tooltip = "Array actions are not permitted when an array is a connection target";
            Path pathToArray = getPathTo(compound);
            const PathStep step = compound->getStepToChild(input);
            const ArrayIndex index = step.getIndex();
            {
                auto insertElement = std::make_unique<ProjectCommandContextMenuAction>(
                    std::make_unique<AddEntriesToArrayCommand>("Add element before", nodeId, pathToArray, index));
                insertElement->setEnabled(arrayActionsAreEnabled && range.contains(currentSize + 1));
                actionsOut.emplace_back(std::move(insertElement));
            }
            {
                auto insertElement = std::make_unique<ProjectCommandContextMenuAction>(
                    std::make_unique<AddEntriesToArrayCommand>("Add element after", nodeId, pathToArray, index + 1));
                insertElement->setEnabled(arrayActionsAreEnabled && range.contains(currentSize + 1));
                actionsOut.emplace_back(std::move(insertElement));
            }
            {
                auto removeElement =
                    std::make_unique<ProjectCommandContextMenuAction>(std::make_unique<RemoveEntryFromArrayCommand>(
                        "Remove array element", nodeId, pathToArray, index, 1));
                removeElement->setEnabled(arrayActionsAreEnabled && range.contains(currentSize - 1));
                actionsOut.emplace_back(std::move(removeElement));
            }
        }
    }
}
