/**
 * The AllToOneFallbackMapEntryModel provides the specific UI for a MapEntry with AllToOneFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

namespace babelwires {
    class AllToOneFallbackMapEntryData;

    /// The AllToOneFallbackMapEntryModel provides the specific UI for a MapEntry with AllToOneFallbackMapEntryData.
    class AllToOneFallbackMapEntryModel : public MapEntryModel {
      public:
        void init() override;
        QVariant getDisplayData(Column column) const override;

        bool isItemEditable(Column column) const override;
        QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;
        void setEditorData(Column column, QWidget* editor) const override;
        std::unique_ptr<MapEntryData> createReplacementDataFromEditor(Column column, QWidget* editor) const override;
        bool validateEditor(QWidget* editor, Column column) const override;

        /*
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */

      private:
        const AllToOneFallbackMapEntryData& getAllToOneFallbackMapEntryData() const;
    };
} // namespace babelwires
