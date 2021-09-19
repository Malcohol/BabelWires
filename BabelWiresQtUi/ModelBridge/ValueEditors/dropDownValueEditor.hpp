/**
 * A dropdown QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ModelBridge/ValueEditors/valueEditorCommonBase.hpp"
#include <QComboBox>

namespace babelwires {

    class DropDownValueEditor : public ValueEditorCommonBase<QComboBox> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        DropDownValueEditor(QWidget* parent, const QModelIndex& index);

        /// Set the text to bold.
        void setFeatureIsModified(bool isModified) override;
    };

} // namespace babelwires
