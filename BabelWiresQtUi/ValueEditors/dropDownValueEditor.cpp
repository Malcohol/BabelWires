/**
 * A combobox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp>

#include <QLineEdit>

babelwires::DropDownValueEditor::DropDownValueEditor(QWidget* parent, const QModelIndex& index)
    : ValueEditorCommonBase(parent, index) {
    setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
                        this, [this]() { emit m_signals->editorHasChanged(this); });

}

void babelwires::DropDownValueEditor::setFeatureIsModified(bool isModified) {
    // TODO: when you click on the row value the value is not bold.
}
