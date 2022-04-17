/**
 * The DiscreteMapEntryModel provides the specific UI for a MapEntry with DiscreteMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp"

namespace babelwires {
    class DiscreteMapEntryData;

    /// The DiscreteMapEntryModel provides the specific UI for a MapEntry with DiscreteMapEntryData.
    class DiscreteMapEntryModel : public MapEntryModel {
      public:
        void init() override;
        QVariant getDisplayData(unsigned int column) const override;

        bool isItemEditable(unsigned int column) const override;
        QWidget* createEditor(const QModelIndex& index, QWidget* parent) const override;
        void setEditorData(unsigned int column, QWidget* editor) const override;
        /*
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */

      private:
        const DiscreteMapEntryData& getDiscreteMapEntryData() const;
    };
} // namespace babelwires
