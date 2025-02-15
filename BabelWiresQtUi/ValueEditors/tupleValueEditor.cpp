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

babelwires::TupleValueEditor::TupleValueEditor(QWidget* parent, const ValueModelRegistry& valueModelRegistry,
                                               const TypeSystem& typeSystem, const TupleType& type, const TupleValue& value)
    : ValueEditorCommonBase(parent)
    , m_tupleType(type)
    , m_tupleValue(value) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);

    auto componentTypes = type.getComponentTypes();
    const unsigned int numComponentTypes = componentTypes.size();
    assert(value.getSize() == numComponentTypes);
    m_perComponentData.resize(numComponentTypes);
    for (auto i = 0; i < numComponentTypes; ++i) {
        const auto& componentType = componentTypes[i].resolve(typeSystem);
        auto& componentValue = value.getValue(i);
        auto& perComponentData = m_perComponentData[i];
        perComponentData.m_valueModel.init(valueModelRegistry, typeSystem, componentType, componentValue, false, false);
        // TODO Don't like passing in a null parent. createEditor should probably not take a parent parameter.
        perComponentData.m_valueEditor = perComponentData.m_valueModel->createEditor(nullptr);
        perComponentData.m_valueModel->setEditorData(perComponentData.m_valueEditor);
        layout->addWidget(perComponentData.m_valueEditor);
        ValueEditorInterface& valueEditorInterface = ValueEditorInterface::getValueEditorInterface(perComponentData.m_valueEditor);
        ValueEditorCommonSignals* editorSignals = valueEditorInterface.getValueEditorSignals();
        QObject::connect(editorSignals, &ValueEditorCommonSignals::editorHasChanged,
            this, [this, i]() { updateValueFromComponentEditor(i); });
    }
}

const babelwires::TupleType& babelwires::TupleValueEditor::getType() const {
    return m_tupleType;
}

const babelwires::TupleValue& babelwires::TupleValueEditor::getEditorData() const {
    return m_tupleValue;
}

void babelwires::TupleValueEditor::setEditorData(const TupleValue& tupleValue) {
    if (m_tupleValue != tupleValue) {
        m_tupleValue = tupleValue;
        for (auto& data : m_perComponentData) {
            data.m_valueModel->setEditorData(data.m_valueEditor);
        }
    }
}

void babelwires::TupleValueEditor::setFeatureIsModified(bool isModified) {
    for (auto& data : m_perComponentData) {
        babelwires::ValueEditorInterface::getValueEditorInterface(data.m_valueEditor).setFeatureIsModified(isModified);
    }
}

void babelwires::TupleValueEditor::updateValueFromComponentEditor(unsigned int i) {
    assert(i < m_perComponentData.size());
    auto& perComponentData = m_perComponentData[i];
    if (EditableValueHolder newValue = perComponentData.m_valueModel->createValueFromEditorIfDifferent(perComponentData.m_valueEditor)) {
        m_tupleValue.setValue(i, newValue);
        emit m_signals->editorHasChanged(this);
    }
}
