/**
 * Base class of models for values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QVariant>

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QPainter;

namespace babelwires {
    class Type;
    class Value;
    class DataLocation;

    class ValueModel {
      public:
        /// For ValueModels which don't use custom painting and return QString the style hint can be ignored since
        /// it will be applied to the font.
        enum class StyleHint { Normal, Bold };
        virtual QVariant getDisplayData(StyleHint styleHint = ValueModel::StyleHint::Normal) const;
        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const;
        virtual void setEditorData(QWidget* editor) const;
        virtual EditableValueHolder createValueFromEditorIfDifferent(QWidget* editor) const;
        virtual bool isItemEditable() const;
        virtual bool validateEditor(QWidget* editor) const;
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QString getTooltip() const;

        /// Add any context actions which should appear in the context menu for this value.
        /// Subclasses overriding this should use super-calls to collect standard actions.
        virtual void getContextMenuActions(const DataLocation& location, std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const;

      protected:
        const Value* getValue() const;
        const Type* getType() const;

      public:
        const Type* m_type;
        const Value* m_value;
        /// Does the context make this value readonly.
        bool m_isReadOnly;
    };
} // namespace babelwires
