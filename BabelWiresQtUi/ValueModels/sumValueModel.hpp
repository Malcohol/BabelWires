/**
 * Model for SumTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

namespace babelwires {

    class ValueModelDispatcher;

    class SumValueModel : ValueModel {
      public:
        bool isItemEditable() const override;
        QWidget* createEditor(QWidget* parent) const override;
        void setEditorData(QWidget* editor) const override;
        EditableValueHolder createValueFromEditorIfDifferent(QWidget* editor) const override;
        bool validateEditor(QWidget* editor) const override;
        void getContextMenuActions(const DataLocation& location, std::vector<ContextMenuEntry>& actionsOut) const override;

      private:
        void initializeValueModelDispatcherForSummand(ValueModelDispatcher& valueModel) const;
    };
} // namespace babelwires
