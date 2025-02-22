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
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenu.hpp>

class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QPainter;

namespace babelwires {
    class Type;
    class Value;
    class TypeSystem;
    class DataLocation;
    class ValueModelRegistry;

    class ValueModel {
      public:
        /// For ValueModels which don't use custom painting and return QString the style hint can be ignored since
        /// it will be applied to the font.
        enum class StyleHint { Normal, Bold };
        virtual QVariant getDisplayData(StyleHint styleHint = ValueModel::StyleHint::Normal) const;
        virtual QWidget* createEditor(QWidget* parent) const;
        virtual void setEditorData(QWidget* editor) const;
        virtual EditableValueHolder createValueFromEditorIfDifferent(QWidget* editor) const;
        virtual bool isItemEditable() const;
        virtual bool validateEditor(QWidget* editor) const;
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option) const;
        virtual QString getTooltip() const;

        /// Add any context actions that should appear in the context menu for this value.
        /// Subclasses overriding this should use super-calls to collect standard actions.
        virtual void getContextMenuActions(const DataLocation& location, std::vector<FeatureContextMenuEntry>& actionsOut) const;

      protected:
        const ValueHolder& getValue() const;
        const Type* getType() const;

      public:
        const TypeSystem* m_typeSystem;
        const Type* m_type;
        const ValueHolder* m_value;
        /// Used by value editors that allow the editing of subvalues, such as tuple.
        const ValueModelRegistry* m_valueModelRegistry;
        /// Does the context make this value readonly.
        bool m_isReadOnly;
        /// Are structural modifications permitted to this value.
        bool m_isStructureEditable;
    };
} // namespace babelwires
