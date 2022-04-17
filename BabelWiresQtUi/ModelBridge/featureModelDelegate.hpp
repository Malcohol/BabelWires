/**
 * FeatureModelDelegate is the QStyledItemDelegate responsible for the editing of entries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QStyledItemDelegate>

namespace babelwires {

    class ProjectBridge;

    class FeatureModelDelegate : public QStyledItemDelegate {
        Q_OBJECT
      public:
        /// Construct a delegate to handle the editing of features.
        FeatureModelDelegate(QObject* parent, ProjectBridge& projectBridge);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;
        void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        bool eventFilter(QObject* object, QEvent* event) override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

      public slots:
        /// When wired to a slot in a row model editor, the editor commits its current value.
        void commitEditorValue();

      private:
        ProjectBridge& m_projectBridge;
    };
} // namespace babelwires
