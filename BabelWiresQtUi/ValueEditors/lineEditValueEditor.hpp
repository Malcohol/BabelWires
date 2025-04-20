/**
 * A line editor QWidget which can be used for editing Values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <QLineEdit>

namespace babelwires {

    /// A line editor QWidget which can be used for editing Values.
    class LineEditValueEditor : public ValueEditorCommonBase<QLineEdit> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        LineEditValueEditor(QWidget* parent);

        /// Set the text to bold.
        void setIsModified(bool isModified) override;
    };

} // namespace babelwires
