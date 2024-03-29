/**
 * The RowModelWithRichText class customizes RowModel to handle embedded html.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelWithRichText.hpp>

#include <QPainter>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QTextDocument>

bool babelwires::RowModelWithRichText::hasCustomPainting() const {
    return true;
}

void babelwires::RowModelWithRichText::paint(QPainter* painter, QStyleOptionViewItem& option,
                                             const QModelIndex& index) const {
    QTextDocument doc;
    doc.setHtml(option.text);

    option.text = "";
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(option.rect.left(), option.rect.top());
    QRect clip(0, 0, option.rect.width(), option.rect.height());
    doc.drawContents(painter, clip);
}

QSize babelwires::RowModelWithRichText::sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const {
    QTextDocument doc;
    doc.setHtml(option.text);
    doc.setTextWidth(option.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}
