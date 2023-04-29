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

class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QPainter;

namespace babelwires {
    class Type;
    class Value;

    class ValueModel {
      public:
        virtual QVariant getDisplayData() const;
        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const;
        virtual void setEditorData(QWidget* editor) const;
        virtual EditableValueHolder createValueFromEditorIfDifferent(QWidget* editor) const;
        virtual bool isItemEditable() const;
        virtual bool validateEditor(QWidget* editor) const;
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
      protected:
        const Value* getValue() const;
        const Type* getType() const;

      public:
        const Type* m_type;
        const Value* m_value;
    };
} // namespace babelwires
