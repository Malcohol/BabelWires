/**
 * Model for EnumValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModel.hpp>

namespace babelwires {

    class EnumValueModel : ValueModel {
      public:
        QWidget* createEditor(const QModelIndex& index, QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
        std::unique_ptr<Value> createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool isItemEditable() const override;
    };
} // namespace babelwires
