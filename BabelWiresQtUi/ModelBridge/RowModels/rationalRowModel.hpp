/**
 * The row model for RationalFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelWithRichText.hpp>

namespace babelwires {

    class RationalFeature;

    /// The row model for RationalFeatures.
    class RationalRowModel : public RowModelWithRichText {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual bool isItemEditable() const override;

        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;

        virtual void setEditorData(QWidget* editor) const override;

        virtual std::unique_ptr<Command<Project>> createCommandFromEditor(QWidget* editor) const override;

      public:
        const RationalFeature& getRationalFeature() const;
    };

} // namespace babelwires
