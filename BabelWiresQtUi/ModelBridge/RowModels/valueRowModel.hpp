/**
 * The row model for ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class ValueFeature;

    class ValueRowModel : public RowModel {
      public:
        void init(const ValueModelRegistry& valueModelRegistry) override;

        virtual QVariant getValueDisplayData() const override;

        virtual bool isItemEditable() const override;

        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;

        virtual void setEditorData(QWidget* editor) const override;

        virtual std::unique_ptr<Command<Project>> createCommandFromEditor(QWidget* editor) const override;

        virtual bool hasCustomPainting() const override;

        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const override;

        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const override;

        virtual QString getTooltip() const override;

        virtual void getContextMenuActions(std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const override;
      public:
        const ValueFeature& getValueFeature() const;
    };

} // namespace babelwires
