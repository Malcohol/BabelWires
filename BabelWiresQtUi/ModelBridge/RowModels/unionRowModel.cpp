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

#include <Common/Identifiers/identifierRegistry.hpp>

#include <QString>

#include <cassert>

const babelwires::UnionFeature& babelwires::UnionRowModel::getUnionFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::UnionFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::UnionFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::UnionRowModel::getValueDisplayData() const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    const Identifier value = unionFeature.getSelectedTag();
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
    const Identifier value = unionFeature.getSelectedTag();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const auto& tags = unionFeature.getTags();
    const auto it = std::find(tags.begin(), tags.end(), value);
    unsigned int currentIndex = it - tags.begin();
    dropDownBox->setCurrentIndex(currentIndex);
}

std::unique_ptr<babelwires::ModifierData> babelwires::UnionRowModel::createModifierFromEditor(QWidget* editor) const {
    const babelwires::UnionFeature& unionFeature = getUnionFeature();
    const auto& tags = unionFeature.getTags();
    const Identifier value = unionFeature.getSelectedTag();
    auto dropDownBox = qobject_cast<DropDownValueEditor*>(editor);
    assert(dropDownBox && "Unexpected editor");
    const int newIndex = dropDownBox->currentIndex();
    assert(newIndex >= 0);
    assert(newIndex < tags.size());
    const Identifier newValue = tags[newIndex];
    if (value != newValue) {
        //auto modifier = std::make_unique<babelwires::EnumValueAssignmentData>();
        //modifier->m_pathToFeature = babelwires::FeaturePath(&unionFeature);
        //modifier->m_value = newValue;
        //return modifier;
    } else {
        return nullptr;
    }
   return nullptr;
}

bool babelwires::UnionRowModel::isItemEditable() const {
    return getInputFeature();
}
