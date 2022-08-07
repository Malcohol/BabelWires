/**
 * The row model for StringFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/stringRowModel.hpp>

#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresLib/Features/stringFeature.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <QString>

#include <cassert>

const babelwires::StringFeature& babelwires::StringRowModel::getStringFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::StringFeature>() &&
           "Wrong type of feature stored");
    return *static_cast<const babelwires::StringFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::StringRowModel::getValueDisplayData() const {
    const babelwires::StringFeature& strFeature = getStringFeature();
    return QString(strFeature.get().c_str());
}

bool babelwires::StringRowModel::isItemEditable() const {
    return getInputFeature();
}

QWidget* babelwires::StringRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    return new LineEditValueEditor(parent, index);
}

void babelwires::StringRowModel::setEditorData(QWidget* editor) const {
    const babelwires::StringFeature& strFeature = getStringFeature();
    std::string value = strFeature.get();
    auto lineEditor = qobject_cast<LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    lineEditor->setText(value.c_str());
}

std::unique_ptr<babelwires::Command<babelwires::Project>> babelwires::StringRowModel::createCommandFromEditor(QWidget* editor) const {
    const babelwires::StringFeature& strFeature = getStringFeature();
    auto lineEditor = qobject_cast<const LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    std::string contents = lineEditor->text().toStdString();
    if (contents != strFeature.get()) {
        auto modifier = std::make_unique<babelwires::StringValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&strFeature);
        modifier->m_value = contents;
        return std::make_unique<AddModifierCommand>("Set string value", m_featureElement->getElementId(),
                                    std::move(modifier));
    } else {
        return nullptr;
    }
}
