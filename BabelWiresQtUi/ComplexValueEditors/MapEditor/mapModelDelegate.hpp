/**
 * MapModel is the QAbstractTableModel which represents the data in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QStyledItemDelegate>

namespace babelwires {

    class MapModelDelegate : public QStyledItemDelegate {
        Q_OBJECT
      public:
        /// Construct a delegate to handle the editing of features.
        MapModelDelegate(QObject* parent);

        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const override;
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
        //virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
        //                   const QModelIndex& index) const override;
        //virtual bool eventFilter(QObject* object, QEvent* event) override;
        //virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

      //public slots:
        /// When wired to a slot in a row model editor, the editor commits its current value.
        //void commitEditorValue();

      //private:
      //  ProjectBridge& m_projectBridge;
    };

}