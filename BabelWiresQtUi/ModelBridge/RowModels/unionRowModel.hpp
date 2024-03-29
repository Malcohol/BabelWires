/**
 * The row model for UnionFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class UnionFeature;

    /// The row model for UnionFeatures.
    class UnionRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual bool isItemEditable() const override;

        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;

        virtual void setEditorData(QWidget* editor) const override;

        virtual std::unique_ptr<Command<Project>> createCommandFromEditor(QWidget* editor) const override;

      public:
        const UnionFeature& getUnionFeature() const;
    };

} // namespace babelwires
