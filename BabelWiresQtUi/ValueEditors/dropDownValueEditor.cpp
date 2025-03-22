/**
 * A combobox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/dropDownValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/Utilities/qtUtils.hpp>

#include <QLineEdit>
#include <QAbstractItemView>

babelwires::DropDownValueEditor::DropDownValueEditor(QWidget* parent)
    : ValueEditorCommonBase(parent) {
    setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
                        this, [this]() { emit m_signals->editorHasChanged(this); });

}

void babelwires::DropDownValueEditor::setFeatureIsModified(bool isModified) {
    // TODO: when you click on the row value the value is not bold.
}

void babelwires::DropDownValueEditor::showPopup() {
    QComboBox::showPopup();
    auto *popup = this->findChild<QFrame*>(); 
    assert(popup && "QComboBox structure not as expected");

    auto* scrollArea = popup->findChild<QAbstractScrollArea*>();
    assert(scrollArea && "QComboBox structure not as expected");
    
    const int maximumHeight = 300;
    popup->setMaximumHeight(maximumHeight);

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Place the popup downwards until it grows to more than half its maximum allowed height.
    const int offsetY = -std::max(0, popup->height() - (maximumHeight / 2));
    QPoint globalPoint = mapToGlobalCorrect(this, QPoint(0, offsetY));
    popup->move(globalPoint);
}
