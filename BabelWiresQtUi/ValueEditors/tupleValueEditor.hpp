/**
 * A spinBox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <QWidget>

namespace babelwires {
    class ValueNames;

    /// 
    class TupleValueEditor : public ValueEditorCommonBase<QWidget> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        TupleValueEditor(QWidget* parent);

        /// Set the text to bold.
        void setFeatureIsModified(bool isModified) override;

    public:
        std::vector<QWidget*> m_componentWidgets;
    };

} // namespace babelwires
