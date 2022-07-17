/**
 * A spinBox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <QSpinBox>

namespace babelwires {
    class ValueNames;

    /// A spinBox QWidget which can be used for editing ValueFeatures.
    // TODO: The handling of valueName text entry is needs work.
    class SpinBoxValueEditor : public ValueEditorCommonBase<QSpinBox> {
        Q_OBJECT
      public:
        /// The arguments as provided to createEditor.
        SpinBoxValueEditor(QWidget* parent, const QModelIndex& index, const ValueNames* valueNames);

        /// Set the text to bold.
        void setFeatureIsModified(bool isModified) override;

        static QString getNamedValueString(const ValueNames* valueNames, int value);

      protected:
        QString textFromValue(int value) const override;
        int valueFromText(const QString &text) const override;
        QValidator::State validate(QString &text, int &pos) const override;

      private:
        const ValueNames* m_valueNames;
    };

} // namespace babelwires
