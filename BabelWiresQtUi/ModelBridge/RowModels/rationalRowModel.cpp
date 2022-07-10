/**
 * The row model for RationalFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/rationalRowModel.hpp"

#include "BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp"

#include "BabelWiresLib/Features/numericFeature.hpp"

#include <QString>
#include <QValidator>

#include <cassert>

const babelwires::RationalFeature& babelwires::RationalRowModel::getRationalFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::RationalFeature>() &&
           "Wrong type of feature stored");
    return *static_cast<const babelwires::RationalFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::RationalRowModel::getValueDisplayData() const {
    const babelwires::RationalFeature& ratFeature = getRationalFeature();
    const Rational value = ratFeature.get();
    if (isFeatureModified()) {
        return QString("<b>") + QString(value.toHtmlString().c_str()) + QString("</b>");
    } else {
        return QString(value.toHtmlString().c_str());
    }
}

bool babelwires::RationalRowModel::isItemEditable() const {
    if (getInputFeature()) {
        return true;
    }
    return false;
}

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

QWidget* babelwires::RationalRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::RationalFeature& ratFeature = getRationalFeature();
    auto editor = new LineEditValueEditor(parent, index);
    editor->setValidator(new RationalValidator);
    // auto range = ratFeature.getRange();
    return editor;
}

void babelwires::RationalRowModel::setEditorData(QWidget* editor) const {
    const babelwires::RationalFeature& ratFeature = getRationalFeature();

    auto lineEditor = qobject_cast<LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    lineEditor->setText(ratFeature.get().toString().c_str());
}

std::unique_ptr<babelwires::ModifierData>
babelwires::RationalRowModel::createModifierFromEditor(QWidget* editor) const {
    const babelwires::RationalFeature& ratFeature = getRationalFeature();
    Rational value = ratFeature.get();
    auto lineEditor = qobject_cast<const LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");

    try {
        value = Rational::parseString(lineEditor->text().toStdString());
    } catch (ParseException& ) {
    }

    if ((value != ratFeature.get()) && ratFeature.getRange().contains(value)) {
        auto modifier = std::make_unique<babelwires::RationalValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&ratFeature);
        modifier->m_value = value;
        return modifier;
    }
    return nullptr;
}
