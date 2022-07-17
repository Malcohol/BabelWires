/**
 * A line editor QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <QLineEdit>

namespace babelwires {

    /// A line editor QWidget which can be used for editing ValueFeatures.
    class LineEditValueEditor : public ValueEditorCommonBase<QLineEdit> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        LineEditValueEditor(QWidget* parent, const QModelIndex& index);

        /// Set the text to bold.
        void setFeatureIsModified(bool isModified) override;
    };

} // namespace babelwires
