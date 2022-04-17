/**
 * Model for IntValues.
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
        QVariant getDisplayData() const override;
        QWidget* createEditor(const QModelIndex& index, QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
    };
} // namespace babelwires
