/**
 * The row model for UnionFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/unionRowModel.hpp>

#include <BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Project/Commands/selectUnionBranchCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <QString>

#include <cassert>

const babelwires::UnionFeature& babelwires::UnionRowModel::getUnionFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::UnionFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::UnionFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::UnionRowModel::getValueDisplayData() const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    const ShortId value = unionFeature.getSelectedTag();
    return QString(IdentifierRegistry::read()->getName(value).c_str());
}

QWidget* babelwires::UnionRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    auto dropDownBox = std::make_unique<DropDownValueEditor>(parent, index);
    {
        IdentifierRegistry::ReadAccess identifierRegistry = IdentifierRegistry::read();
        for (auto enumValue : unionFeature.getTags()) {
            dropDownBox->addItem(identifierRegistry->getName(enumValue).c_str());
        }
    }
    return dropDownBox.release();
}

void babelwires::UnionRowModel::setEditorData(QWidget* editor) const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    const ShortId value = unionFeature.getSelectedTag();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    dropDownBox->setCurrentIndex(unionFeature.getSelectedTagIndex());
}

std::unique_ptr<babelwires::Command<babelwires::Project>> babelwires::UnionRowModel::createCommandFromEditor(QWidget* editor) const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    const auto& tags = unionFeature.getTags();
    const ShortId value = unionFeature.getSelectedTag();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < tags.size());
    const ShortId newValue = tags[newIndex];
    if (value != newValue) {
        return std::make_unique<SelectUnionBranchCommand>("Select union branch", m_featureElement->getElementId(), babelwires::FeaturePath(&unionFeature), newValue);
    } else {
        return nullptr;
    }
   return nullptr;
}

bool babelwires::UnionRowModel::isItemEditable() const {
    return getInputFeature();
}
