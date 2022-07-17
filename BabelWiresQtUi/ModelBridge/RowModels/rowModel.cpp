/**
 * Base class of models corresponding to a row in a node of the flow graph.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/insertArrayEntryAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeArrayEntryAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeFailedModifiersAction.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeModifierAction.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp>

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>

#include <QBrush>

#include <cassert>

bool babelwires::RowModel::hasInputFeature() const {
    return m_contentsCacheEntry->getInputFeature();
}

const babelwires::Feature* babelwires::RowModel::getInputFeature() const {
    return m_contentsCacheEntry->getInputFeature();
}

const babelwires::Feature* babelwires::RowModel::getOutputFeature() const {
    return m_contentsCacheEntry->getOutputFeature();
}

const babelwires::Feature* babelwires::RowModel::getInputThenOutputFeature() const {
    return m_contentsCacheEntry->getInputThenOutputFeature();
}

const babelwires::Feature* babelwires::RowModel::getOutputThenInputFeature() const {
    return m_contentsCacheEntry->getOutputThenInputFeature();
}

bool babelwires::RowModel::isFeatureModified() const {
    assert(!m_contentsCacheEntry->hasModifier() ||
           hasInputFeature() && "It does not make sense for a modifier to be present when there is no input feature");
    return m_contentsCacheEntry->hasModifier();
}

QVariant babelwires::RowModel::getValueDisplayData() const {
    return {};
}

QVariant babelwires::RowModel::getTooltip() const {
    if (m_featureElement->isFailed()) {
        return m_featureElement->getReasonForFailure().c_str();
    } else if (m_contentsCacheEntry->hasFailedModifier()) {
        const Modifier* modifier = m_featureElement->findModifier(m_contentsCacheEntry->getPath());
        assert(modifier && "The cache is inconsistent with the data");
        assert(modifier->isFailed() && "The cache is inconsistent with the data");
        return modifier->getReasonForFailure().c_str();
    } else if (m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        QString message = "This feature contains failed modifiers:";
        int numFailedModifiers = 0;
        for (const Modifier* modifier : m_featureElement->getEdits().modifierRange(m_contentsCacheEntry->getPath())) {
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
    if (m_featureElement->isFailed()) {
        return BackgroundStyle::failed;
    } else if (m_contentsCacheEntry->hasFailedModifier()) {
        return BackgroundStyle::failed;
    } else if (m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        return BackgroundStyle::failedHidden;
    } else if ((c == ColumnType::Value) && hasInputFeature()) {
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

std::unique_ptr<babelwires::ModifierData> babelwires::RowModel::createModifierFromEditor(QWidget* editor) const {
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

void babelwires::RowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    if (isFeatureModified()) {
        actionsOut.emplace_back(std::make_unique<RemoveModifierAction>());
    }
    if (m_contentsCacheEntry->hasFailedModifier() || m_contentsCacheEntry->hasFailedHiddenModifiers()) {
        actionsOut.emplace_back(std::make_unique<RemoveFailedModifiersAction>());
    }
    if (const babelwires::Feature* inputFeature = getInputFeature()) {
        if (auto arrayFeature = inputFeature->getOwner()->as<const ArrayFeature>()) {
            FeaturePath pathToArray(arrayFeature);
            const PathStep step = arrayFeature->getStepToChild(inputFeature);
            const ArrayIndex index = step.getIndex();
            const auto sizeRange = arrayFeature->getSizeRange();
            const auto currentSize = arrayFeature->getNumFeatures();
            {
                auto insertElement = std::make_unique<InsertArrayEntryAction>("Add element before", pathToArray, index);
                insertElement->setEnabled(sizeRange.contains(currentSize + 1));
                actionsOut.emplace_back(std::move(insertElement));
            }
            {
                auto insertElement = std::make_unique<InsertArrayEntryAction>("Add element after", pathToArray, index + 1);
                insertElement->setEnabled(sizeRange.contains(currentSize + 1));
                actionsOut.emplace_back(std::move(insertElement));
            }
            {
                auto removeElement = std::make_unique<RemoveArrayEntryAction>(pathToArray, index);
                removeElement->setEnabled(sizeRange.contains(currentSize - 1));
                actionsOut.emplace_back(std::move(removeElement));
            }
        }
    }
}
