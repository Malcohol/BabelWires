/**
 * Model for StringValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {

    class StringValueModel : ValueModel {
      public:
        bool isItemEditable() const override;
        QWidget* createEditor(QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
        EditableValueHolder createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool validateEditor(QWidget* editor) const override;
    };
} // namespace babelwires
