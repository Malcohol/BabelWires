/**
 * Model for Tuples.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {

    class TupleValueModel : ValueModel {
      public:
        bool isItemEditable() const override;
        QWidget* createEditor(QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
        ValueHolder createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool validateEditor(QWidget* editor) const override;
    };
} // namespace babelwires
