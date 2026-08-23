/**
 * Model for FixedValues.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/fixedValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresLib/Types/Fixed/fixedType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <QString>
#include <QValidator>

namespace {

    struct FixedValidator : public QValidator {
        FixedValidator(int expectedPrecision) : m_expectedPrecision(expectedPrecision) {}
        virtual QValidator::State validate(QString& input, int& pos) const override {
            const std::string text = input.toStdString();
            if (text.empty()) {
                return QValidator::Intermediate;
            }
            babelwires::Fixed dummy;
            const auto parseResult = babelwires::Fixed::partialParse(text, dummy, m_expectedPrecision);
            if (parseResult == babelwires::Fixed::PartialParseResult::Success) {
                return QValidator::Acceptable;
            } else if (parseResult == babelwires::Fixed::PartialParseResult::Truncated) {
                return QValidator::Intermediate;
            } else {
                return QValidator::Invalid;
            }
        }

        int m_expectedPrecision = -1;
    };

} // namespace

QWidget* babelwires::FixedValueModel::createEditor(QWidget* parent) const {
    auto editor = new LineEditValueEditor(parent);
    const auto fixedType = getType()->as<FixedType>();
    editor->setValidator(new FixedValidator(fixedType.getPrecision()));
    return editor;
}

void babelwires::FixedValueModel::setEditorData(QWidget* editor) const {
    const FixedValue& v = getValue()->as<FixedValue>();
    const Fixed value = v.get();

    auto lineEditor = qobject_cast<LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    lineEditor->setText(QString::fromStdString(value.toString()));
}

babelwires::ValueHolder babelwires::FixedValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    const FixedValue& v = getValue()->as<FixedValue>();
    const FixedType& fixedType = getType()->as<FixedType>();
    const Fixed currentValue = v.get();

    auto lineEditor = qobject_cast<const LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");

    Fixed newValue = currentValue;
    const std::string text = lineEditor->text().toStdString();
#ifndef NDEBUG
    const auto parseResult = 
#endif // NDEBUG
    Fixed::partialParse(text, newValue, fixedType.getPrecision());
    assert(parseResult == Fixed::PartialParseResult::Success);

    if (newValue != currentValue) {
        return ValueHolder::makeValue<babelwires::FixedValue>(newValue);
    }
    return {};
}

bool babelwires::FixedValueModel::isItemEditable() const {
    return getValue()->tryAs<FixedValue>();
}

bool babelwires::FixedValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<LineEditValueEditor*>(editor);
}
