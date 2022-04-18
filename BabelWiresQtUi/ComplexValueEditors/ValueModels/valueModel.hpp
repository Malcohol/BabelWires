/**
 * Base class of models for values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QVariant>

#include <memory>

namespace babelwires {
    class Type;
    class Value;

    class ValueModel {
      public:
        virtual QVariant getDisplayData() const;
        virtual QWidget* createEditor(const QModelIndex& index, QWidget* parent) const;
        virtual void setEditorData(QWidget* editor) const;
        virtual std::unique_ptr<Value> getValueFromEditor(QWidget* editor) const;
        /*
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */
      protected:
        const Value* getValue() const;
        const Type* getType() const;

      public:
        const Type* m_type;
        const Value* m_value; 
    };
} // namespace babelwires
