/**
 * The row model for StringFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class StringFeature;

    /// The RowModel for StringFeatures.
    class StringRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual bool isItemEditable() const override;

        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;

        virtual void setEditorData(QWidget* editor) const override;

        virtual std::unique_ptr<ModifierData> createModifierFromEditor(QWidget* editor) const override;

      public:
        const StringFeature& getStringFeature() const;
    };

} // namespace babelwires
