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
    // Store a reference to the interface this class implements via a property in the QObject.
    QVariant property;
    property.setValue(static_cast<ValueEditorInterface*>(this));
    this->setProperty(ValueEditorInterface::s_propertyName, property);
}

template <typename T>
QMetaObject::Connection& babelwires::ValueEditorCommonBase<T>::getValuesChangedConnection() {
    return m_valuesChangedConnection;
}

template <typename T> babelwires::ValueEditorCommonBase<T>::~ValueEditorCommonBase() {
    if (m_valuesChangedConnection) {
        QObject::disconnect(m_valuesChangedConnection);
    }
}
