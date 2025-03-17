/**
 * Model for RationalValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/rationalValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <QString>
#include <QValidator>

namespace {

    /// Only allow valid rationals to be entered.
    struct RationalValidator : public QValidator {
        virtual QValidator::State validate(QString& input, int& pos) const override {
            babelwires::Rational dummy;
            switch (babelwires::Rational::partialParse(input.toStdString(), dummy)) {
                case babelwires::Rational::PartialParseResult::Success:
                    return QValidator::Acceptable;
                case babelwires::Rational::PartialParseResult::Failure:
                    return QValidator::Invalid;
                case babelwires::Rational::PartialParseResult::Truncated:
                    return QValidator::Intermediate;
            }
            return QValidator::Intermediate;
        }
    };

} // namespace

QString babelwires::RationalValueModel::getRichText() const {
    const Rational value = getValue()->is<RationalValue>().get();
    return value.toHtmlString().c_str();
}

QWidget* babelwires::RationalValueModel::createEditor(QWidget* parent) const {
    auto editor = new LineEditValueEditor(parent);
    editor->setValidator(new RationalValidator);
    //auto range = getType()->is<RationalType>().getRange();
    return editor;
}

void babelwires::RationalValueModel::setEditorData(QWidget* editor) const {
    const RationalValue& v = getValue()->is<RationalValue>();
    const Rational value = v.get();

    auto lineEditor = qobject_cast<LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    lineEditor->setText(v.get().toString().c_str());
}

babelwires::EditableValueHolder babelwires::RationalValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    const RationalValue& v = getValue()->is<RationalValue>();
    const Rational currentValue = v.get();

    auto lineEditor = qobject_cast<const LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");

    Rational newValue = currentValue;
    try {
        newValue = Rational::parseString(lineEditor->text().toStdString());
    } catch (ParseException& ) {
    }

    if (newValue != currentValue) {
        return EditableValueHolder::makeValue<babelwires::RationalValue>(newValue);
    }
    return {};
}

bool babelwires::RationalValueModel::isItemEditable() const {
    return getValue()->as<RationalValue>();
}

bool babelwires::RationalValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<LineEditValueEditor*>(editor);
}
