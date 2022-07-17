/**
 * The row model for EnumFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class EnumFeature;

    /// The row model for IntFeatures.
    class EnumRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual bool isItemEditable() const override;

        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;

        virtual void setEditorData(QWidget* editor) const override;

        virtual std::unique_ptr<ModifierData> createModifierFromEditor(QWidget* editor) const override;

      public:
        const EnumFeature& getEnumFeature() const;
    };

} // namespace babelwires
