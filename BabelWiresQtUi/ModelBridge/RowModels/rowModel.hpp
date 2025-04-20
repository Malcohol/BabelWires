/**
 * Base class of models corresponding to a row in a node of the flow graph.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <BabelWiresQtUi/ValueModels/valueModelDispatcher.hpp>
#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>

#include <QVariant>

#include <memory>
#include <vector>

class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QPainter;

namespace babelwires {

    class ValueTreeNode;
    class Node;
    class ContentsCacheEntry;
    class Modifier;

    /// Base class of models corresponding to a row in a node of the flow graph.
    /// Such objects are created as temporaries, and should not have state.
    class RowModel {
      public:
        // Note: No virtual destructor.
        virtual void init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem);

        virtual QVariant getValueDisplayData() const;

        virtual QString getTooltip() const;

        enum class ColumnType { Key, Value };

        enum class BackgroundStyle {
            normal,
            editable,
            failed,
            // There's a failed modifier which is hidden because a containing ValueTreeNode is collapsed in the UI.
            failedHidden
        };

        virtual BackgroundStyle getBackgroundStyle(ColumnType c) const;

        /// Only called for input ValueTreeNodes.
        virtual bool isItemEditable() const;

        /// Return an editor for editing the value.
        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const;

        /// Set the data in the editor.
        virtual void setEditorData(QWidget* editor) const;

        /// Given that the user has committed the edit, return a new command performing that edit.
        virtual std::unique_ptr<Command<Project>> createCommandFromEditor(QWidget* editor) const;

        /// Returns true if this RowModel overrides paint and sizeHint.
        /// The default implementation returns false.
        virtual bool hasCustomPainting() const;

        /// Option is a copy and may safely be modified.
        /// Painter is already saved and will be restored after this call.
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;

        /// Required when custom painting.
        /// Option is a copy and may safely be modified.
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;

        /// Add any context actions which should appear in the context menu for this row.
        /// Subclasses overriding this should use super-calls to collect standard actions.
        virtual void getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const;

      public:
        bool isModified() const;

      protected:
        bool hasInput() const;
        const ValueTreeNode* getInput() const;
        const ValueTreeNode* getOutput() const;
        const ValueTreeNode* getInputThenOutput() const;
        const ValueTreeNode* getOutputThenInput() const;

      public:
        const ContentsCacheEntry* m_contentsCacheEntry = nullptr;
        const Node* m_node = nullptr;
        ValueModelDispatcher m_valueModelDispatcher;
    };

} // namespace babelwires
