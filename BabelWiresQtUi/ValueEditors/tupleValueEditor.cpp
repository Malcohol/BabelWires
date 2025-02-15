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
    , m_tupleValue(value) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);

    auto componentTypes = type.getComponentTypes();
    const unsigned int numComponentTypes = componentTypes.size();
    assert(value.getSize() == numComponentTypes);
    m_perComponentData.resize(numComponentTypes);
    for (auto i = 0; i < numComponentTypes; ++i) {
        const auto& componentType = componentTypes[i].resolve(typeSystem);
        auto componentValue = value.getValue(i);
        auto& perComponentData = m_perComponentData[i];
        perComponentData.m_valueModel.init(valueModelRegistry, typeSystem, componentType, componentValue, false, false);
        perComponentData.m_valueEditor = perComponentData.m_valueModel->createEditor(nullptr);
        layout->addWidget(perComponentData.m_valueEditor);
    }
    // Commit on select.
    // QObject::connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
    //                    this, [this]() { emit m_signals->editorHasChanged(this); });
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
        babelwires::ValueEditorInterface::getValueEditorInterface(data.m_valueEditor)->setFeatureIsModified(isModified);
    }
}
