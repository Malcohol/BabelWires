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
        /// Wraps the result of getRichText with <b> and </b> if indicated by the styleHint.
        QVariant getDisplayData(StyleHint styleHint) const override final;

        virtual bool hasCustomPainting() const override;

        virtual void paint(QPainter* painter, QStyleOptionViewItem& option) const override;

        virtual QSize sizeHint(QStyleOptionViewItem& option) const override;

        /// Subclasses must provide rich text for use by getDisplayData.
        virtual QString getRichText() const = 0;
    };

} // namespace babelwires
