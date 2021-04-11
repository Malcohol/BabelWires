/**
 * Common functionality for editors which allow the user to edit ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T>
babelwires::ValueEditorCommonBase<T>::ValueEditorCommonBase(QWidget* parent, const QModelIndex& index)
    : T(parent) {
    // Update the editor if the model changes.
    // The primary motivation is making the comboBox go bold after a selection is made.
    // However, it seems generally desirable.
    m_valuesChangedConnection = QObject::connect(
        RowModel::getModelFromParentWidget(parent), &FeatureModel::valuesMayHaveChanged,
        [this, parent, index]() { emit RowModel::getDelegateFromParentWidget(parent)->setEditorData(this, index); });

    // Store a reference to the interface this class implements via a property in the QObject.
    QVariant property;
    property.setValue(static_cast<ValueEditorInterface*>(this));
    this->setProperty(ValueEditorInterface::s_propertyName, property);
}

template <typename T> babelwires::ValueEditorCommonBase<T>::~ValueEditorCommonBase() {
    if (m_valuesChangedConnection) {
        QObject::disconnect(m_valuesChangedConnection);
    }
}
