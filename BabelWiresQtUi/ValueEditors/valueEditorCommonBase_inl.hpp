/**
 * Common functionality for editors which allow the user to edit Values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T>
babelwires::ValueEditorCommonBase<T>::ValueEditorCommonBase(QWidget* parent)
    : T(parent)
    , m_signals(new ValueEditorCommonSignals(static_cast<T*>(this))) {
    // Store a reference to the interface this class implements via a property in the QObject.
    QVariant property;
    property.setValue(static_cast<ValueEditorInterface*>(this));
    this->setProperty(ValueEditorInterface::s_propertyName, property);
}

template <typename T>
QMetaObject::Connection& babelwires::ValueEditorCommonBase<T>::getValuesChangedConnection() {
    return m_valuesChangedConnection;
}

template <typename T>
babelwires::ValueEditorCommonSignals* babelwires::ValueEditorCommonBase<T>::getValueEditorSignals() {
    return m_signals;
}

template <typename T> babelwires::ValueEditorCommonBase<T>::~ValueEditorCommonBase() {
    if (m_valuesChangedConnection) {
        QObject::disconnect(m_valuesChangedConnection);
    }
}
