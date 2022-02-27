/**
 * FeatureModel is the QAbstractTableModel which represents the data in a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectIds.hpp"

#include <QAbstractTableModel>
#include <QAction>
#include <QMenu>
#include <QStyledItemDelegate>
#include <QTableView>

namespace babelwires {

    class ContentsCache;
    class FeatureElement;
    struct ContentsCacheEntry;
    class ProjectBridge;
    class AccessModelScope;
    class FeaturePath;

    class FeatureView : public QTableView {
        Q_OBJECT
      public:
        FeatureView(ElementId elementId, ProjectBridge& projectBridge);

        /// The default size is too big when there are three or fewer rows.
        QSize sizeHint() const override;

        /// The default size is too big when there are three or fewer rows.
        QSize minimumSizeHint() const override;

      private:
        ProjectBridge& m_projectBridge;
        ElementId m_elementId;
    };

    /// Presents the contents of the contentsCache as a table model.
    class FeatureModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        FeatureModel(QObject* parent, ElementId elementId, ProjectBridge& projectBridge);

        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        /// Access the associated FeatureElement.
        const FeatureElement* getFeatureElement(AccessModelScope& scope) const;
        /// Access an entry in the ContentsCache.
        const ContentsCacheEntry* getEntry(AccessModelScope& scope, const QModelIndex& index) const;
        const ContentsCacheEntry* getEntry(AccessModelScope& scope, int row) const;
        /// Access the number of rows.
        int getNumRows(AccessModelScope& scope) const;

        /// May return null or a new menu.
        QMenu* getContextMenu(const QModelIndex& index);

        ProjectBridge& getProjectBridge();

        ElementId getElementId() const;

      public slots:
        void onClicked(const QModelIndex& index) const;

      signals:
        void valuesMayHaveChanged() const;

      private:
        ProjectBridge& m_projectBridge;
        ElementId m_elementId;
    };

    class FeatureModelDelegate : public QStyledItemDelegate {
        Q_OBJECT
      public:
        /// Construct a delegate to handle the editing of features.
        /// The elementId is that of the FeatureElement.
        FeatureModelDelegate(QObject* parent, ProjectBridge& projectBridge);

        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const override;
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const override;
        virtual bool eventFilter(QObject* object, QEvent* event) override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

      public slots:
        /// When wired to a slot in a row model editor, the editor commits its current value.
        void commitEditorValue();

      private:
        ProjectBridge& m_projectBridge;
    };

} // namespace babelwires