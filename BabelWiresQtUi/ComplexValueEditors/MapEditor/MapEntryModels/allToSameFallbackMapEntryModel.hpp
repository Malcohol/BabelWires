/**
 * The AllToSameFallbackMapEntryModel provides the specific UI for a MapEntry with AllToSameFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

namespace babelwires {
    class AllToSameFallbackMapEntryData;

    /// The AllToSameFallbackMapEntryModel provides the specific UI for a MapEntry with AllToSameFallbackMapEntryData.
    class AllToSameFallbackMapEntryModel : public MapEntryModel {
      public:
        QVariant getDisplayData() const override;
        /*
        virtual bool isItemEditable() const;
        virtual QWidget* createEditor(QWidget* parent, const QModelIndex& index) const;
        virtual void setEditorData(QWidget* editor) const;
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */
    };
} // namespace babelwires
