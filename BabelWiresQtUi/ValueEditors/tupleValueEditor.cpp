/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/tupleValueEditor.hpp>

#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <QHBoxLayout>
#include <QLabel>

babelwires::TupleValueEditor::TupleValueEditor(QWidget* parent, const ValueModelRegistry& valueModelRegistry,
                                               const TypeSystem& typeSystem, const TupleType& type, const TupleValue& value)
    : ValueEditorCommonBase(parent)
    , m_tupleType(type) {
    setAutoFillBackground(true);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);

    auto addSeparator = [layout](const char* text) {
        QLabel* label = new QLabel(text);
        label->setMaximumWidth(label->minimumSizeHint().width());
        layout->addWidget(label);
    };
    addSeparator("(");

    auto componentTypes = type.getComponentTypes();
    const unsigned int numComponentTypes = componentTypes.size();
    assert(value.getSize() == numComponentTypes);
    m_perComponentData.resize(numComponentTypes);
    for (auto i = 0; i < numComponentTypes; ++i) {
        if (i > 0) {
            addSeparator(",");
        }
        auto& componentValue = value.getValue(i);
        auto& perComponentData = m_perComponentData[i];
        perComponentData.m_value = componentValue;
        perComponentData.m_valueModel.init(valueModelRegistry, typeSystem, componentTypes[i], perComponentData.m_value, false, false);
        // TODO Don't like passing in a null parent. createEditor should probably not take a parent parameter.
        perComponentData.m_valueEditor = perComponentData.m_valueModel->createEditor(nullptr);
        perComponentData.m_valueModel->setEditorData(perComponentData.m_valueEditor);
        layout->addWidget(perComponentData.m_valueEditor);
        ValueEditorInterface& valueEditorInterface = ValueEditorInterface::getValueEditorInterface(perComponentData.m_valueEditor);
        ValueEditorCommonSignals* editorSignals = valueEditorInterface.getValueEditorSignals();
        QObject::connect(editorSignals, &ValueEditorCommonSignals::editorHasChanged,
            this, [this, i]() { updateValueFromComponentEditor(i); });
    }
    addSeparator(")");
}

const babelwires::TupleType& babelwires::TupleValueEditor::getType() const {
    return m_tupleType;
}

babelwires::TupleValue babelwires::TupleValueEditor::getEditorData() const {
    TupleValue::Tuple newTuple;
    newTuple.reserve(m_perComponentData.size());
    for (const auto& perComponentData : m_perComponentData) {
        newTuple.emplace_back(perComponentData.m_value);
    }
    return { newTuple };
}

void babelwires::TupleValueEditor::setEditorData(const TupleValue& tupleValue) {
    for (int i = 0; i < m_perComponentData.size(); ++i) {
        PerComponentData& perComponentData = m_perComponentData[i];
        perComponentData.m_value = tupleValue.getValue(i);
        perComponentData.m_valueModel->setEditorData(perComponentData.m_valueEditor);
    }
}

void babelwires::TupleValueEditor::setIsModified(bool isModified) {
    for (auto& data : m_perComponentData) {
        babelwires::ValueEditorInterface::getValueEditorInterface(data.m_valueEditor).setIsModified(isModified);
    }
}

void babelwires::TupleValueEditor::updateValueFromComponentEditor(unsigned int i) {
    assert(i < m_perComponentData.size());
    auto& perComponentData = m_perComponentData[i];
    if (ValueHolder newValue = perComponentData.m_valueModel->createValueFromEditorIfDifferent(perComponentData.m_valueEditor)) {
        perComponentData.m_value = newValue;
        emit m_signals->editorHasChanged(this);
    }
}
