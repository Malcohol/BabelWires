/**
 * The ValueModelWithRichText class customizes ValueModel to handle embedded html.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {

    /// Will handle displayData with embedded html.
    class ValueModelWithRichText : public ValueModel {
      public:
        virtual bool hasCustomPainting() const override;

        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const override;

        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const override;
    };

} // namespace babelwires
