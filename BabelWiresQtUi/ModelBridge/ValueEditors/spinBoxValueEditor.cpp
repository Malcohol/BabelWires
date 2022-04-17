/**
 * A spinBox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ValueEditors/spinBoxValueEditor.hpp"

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp"

#include "BabelWiresLib/ValueNames/valueNames.hpp"

#include <QLineEdit>


namespace {
    QString getNamedValueString(int value, const std::string& name) {
        return QString("%2 (%1)").arg(name.c_str()).arg(value);
    }

    bool getValueFromString(const babelwires::ValueNames& valueNames, QString str, int& newValue) {
        bool success = false;
        str = str.trimmed();
        newValue = str.toInt(&success);
        if (success) {
            return true;
        } else if (valueNames.getValueForName(str.toStdString(), newValue)) {
            return true;
        } else {
            const QRegExp re("^([0-9]*)\\s*\\((.*)\\)$");
            if (re.indexIn(str) != -1) {
                bool intPartSuccess = false;
                const int intPart = re.cap(1).toInt(&intPartSuccess);
                if (intPartSuccess) {
                    int namePart;
                    if (valueNames.getValueForName(re.cap(2).toStdString(), namePart)) {
                        if (intPart == namePart) {
                            newValue = intPart;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
}

babelwires::SpinBoxValueEditor::SpinBoxValueEditor(QWidget* parent, const QModelIndex& index,
                                                   const ValueNames* valueNames)
    : ValueEditorCommonBase(parent, index)
    , m_valueNames(valueNames) {
    // Commit on select.
    QObject::connect(this, QOverload<int>::of(&QSpinBox::valueChanged), 
                        this, [this]() { emit m_signals->editorHasChanged(this); });
}

void babelwires::SpinBoxValueEditor::setFeatureIsModified(bool isModified) {
    QLineEdit* lineEditor = lineEdit();
    QFont font = lineEditor->font();
    font.setBold(isModified);
    lineEditor->setFont(font);
}

QString babelwires::SpinBoxValueEditor::getNamedValueString(const ValueNames* valueNames, int value) {
    if (valueNames) {
        std::string name;
        if (valueNames->getNameForValue(value, name)) {
            return QString("%2 (%1)").arg(name.c_str()).arg(value);
        }
    }
    return QString("%1").arg(value);
}

QString babelwires::SpinBoxValueEditor::textFromValue(int value) const {
    return getNamedValueString(m_valueNames, value);
}

int babelwires::SpinBoxValueEditor::valueFromText(const QString& text) const {
    if (m_valueNames) {
        int value;
        if (getValueFromString(*m_valueNames, text, value)) {
            return value;
        }
    }
    return QSpinBox::valueFromText(text);
}

QValidator::State babelwires::SpinBoxValueEditor::validate(QString &text, int &pos) const {
    if (m_valueNames) {
        QString textIn = text;
        textIn.truncate(pos);
        for (auto vn : *m_valueNames) {
            const QString name = std::get<1>(vn).c_str();
            if (textIn == name) {
                return QValidator::State::Acceptable;
            } else if (name.startsWith(textIn)) {
                return QValidator::State::Intermediate;
            }
        }
    }
    return QSpinBox::validate(text, pos);
}
