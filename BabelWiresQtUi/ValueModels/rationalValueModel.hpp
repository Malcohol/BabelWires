/**
 * Model for RationalValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModelWithRichText.hpp>

namespace babelwires {

    class RationalValueModel : ValueModelWithRichText {
      public:
        QString getRichText() const override;
        bool isItemEditable() const override;
        QWidget* createEditor(QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
        ValueHolder createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool validateEditor(QWidget* editor) const override;
    };
} // namespace babelwires
