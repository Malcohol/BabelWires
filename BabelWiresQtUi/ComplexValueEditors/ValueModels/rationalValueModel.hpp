/**
 * Model for RationalValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModel.hpp>

namespace babelwires {

    class RationalValueModel : ValueModel {
      public:
        bool isItemEditable() const override;
        QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;
        void setEditorData(QWidget* editor) const override;
        std::unique_ptr<Value> createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool validateEditor(QWidget* editor) const override;
    };
} // namespace babelwires
