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

#include <QAbstractItemView>
#include <QLineEdit>

babelwires::DropDownValueEditor::DropDownValueEditor(QWidget* parent)
    : ValueEditorCommonBase(parent) {
    setAutoFillBackground(true);
    setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     [this]() { emit m_signals->editorHasChanged(this); });
}

void babelwires::DropDownValueEditor::setFeatureIsModified(bool isModified) {
    // TODO: when you click on the row value the value is not bold.
}

void babelwires::DropDownValueEditor::showPopup() {
    QComboBox::showPopup();
    auto* popup = this->findChild<QFrame*>();
    assert(popup && "QComboBox structure not as expected");

    // The default behaviour for dropdowns in the graphicsview does not limit the size of the popup,
    // which is unusable for large enums.
    const int maximumHeight = 300;
    popup->setMaximumHeight(maximumHeight);

    // The popup's default position can be way off, so we have to explicitly set it.
    // Allow the popup to grow downwards until it grows to reaches half its maximum allowed height.
    const int offsetY = -std::max(0, popup->height() - (maximumHeight / 2));
    const QPoint localPopupPosition = QPoint(0, offsetY);
    const QPoint globalPopupPosition = mapToGlobalCorrect(this, localPopupPosition);
    // Note: There seems to be a (Qt?) bug where the move doesn't quite place the popup in the correct
    // vertical position, despite the horizontal position being correct. This is particularly visible
    // when zoomed out. Using a fixed point shows that that globalPopupPosition is not to blame.
    popup->move(globalPopupPosition);

    // Ensure the popup gets a scroll bar when needed.
    {
        auto* scrollArea = popup->findChild<QAbstractScrollArea*>();
        assert(scrollArea && "QComboBox structure not as expected");
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}
