/**
 * MapEntryModels provide the UI specifics for MapEntries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QVariant>

#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModelDispatcher.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

namespace babelwires {
    class Type;
    class MapProjectEntry;
    class MapEntryData;

    /// MapEntryModels provide the UI specifics for MapEntries.
    /// The class should always be constructed via the MapEntryModelDispatcher.
    class MapEntryModel {
      public:
        virtual void init();

        enum class Column
        {
          sourceValue = 0,
          targetValue = 1
        };

        virtual QVariant getDisplayData(Column column) const;

        /// Add any context actions which should appear in the context menu for this row.
        /// Subclasses overriding this should use super-calls to collect standard actions.
        virtual void getContextMenuActions(std::vector<std::unique_ptr<ContextMenuAction>>& actionsOut) const;

        /// The default returns false.
        virtual bool isItemEditable(Column column) const;

        /// The default asserts.
        virtual QWidget* createEditor(const QModelIndex& index, QWidget* parent) const;
        
        /// The default asserts.
        virtual void setEditorData(Column column, QWidget* editor) const;
        
        virtual std::unique_ptr<MapEntryData> createReplacementDataFromEditor(Column column, QWidget* editor) const;

        /// Check whether the editor is valid.
        virtual bool validateEditor(QWidget* editor, Column column) const;

        /*
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */
      public:
        const Type* m_sourceType;
        const Type* m_targetType; 
        const MapProjectEntry* m_mapProjectEntry;
        unsigned int m_row = 0;
        bool m_isLastRow = false;

        // If needed, a ValueModelDispatcher for the map entry source.
        ValueModelDispatcher m_sourceValueModel;

        // If needed, a ValueModelDispatcher for the map entry target.
        ValueModelDispatcher m_targetValueModel;
    };
} // namespace babelwires
