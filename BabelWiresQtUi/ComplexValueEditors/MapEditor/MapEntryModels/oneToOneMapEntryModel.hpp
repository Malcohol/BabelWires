/**
 * The OneToOneMapEntryModel provides the specific UI for a MapEntry with OneToOneMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

namespace babelwires {
    class OneToOneMapEntryData;

    /// The OneToOneMapEntryModel provides the specific UI for a MapEntry with OneToOneMapEntryData.
    class OneToOneMapEntryModel : public MapEntryModel {
      public:
        void init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem) override;
        QVariant getDisplayData(Column column) const override;

        bool isItemEditable(Column column) const override;
        QWidget* createEditor(QWidget* parent, const QModelIndex& index) const override;
        void setEditorData(Column column, QWidget* editor) const override;
        std::unique_ptr<MapEntryData> createReplacementDataFromEditor(Column column, QWidget* editor) const override;
        bool validateEditor(QWidget* editor, Column column) const override;
        void getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const override;

        /*
        virtual bool hasCustomPainting() const;
        virtual void paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual QSize sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const;
        */

      private:
        const OneToOneMapEntryData& getOneToOneMapEntryData() const;
    };
} // namespace babelwires
