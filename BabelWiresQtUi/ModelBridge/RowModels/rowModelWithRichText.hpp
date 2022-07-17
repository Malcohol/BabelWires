/**
 * The RowModelWithRichText class customizes RowModel to handle embedded html.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    /// Will handle displayData with embedded html.
    class RowModelWithRichText : public RowModel {
      public:
        virtual bool hasCustomPainting() const override;

        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const override;

        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const override;
    };

} // namespace babelwires
