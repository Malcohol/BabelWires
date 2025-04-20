/**
 * A dropdown QWidget which can be used for editing Values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <QComboBox>

namespace babelwires {

    class DropDownValueEditor : public ValueEditorCommonBase<QComboBox> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        DropDownValueEditor(QWidget* parent);

        /// Set the text to bold.
        void setIsModified(bool isModified) override;

        // This is overridden to ensure the popup does not have an excessive height.
        void showPopup() override;
    };

} // namespace babelwires
