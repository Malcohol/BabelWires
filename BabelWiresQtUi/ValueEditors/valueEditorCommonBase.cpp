/**
 * Common functionality for editors which allow the user to edit ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <QWidget>

babelwires::ValueEditorCommonSignals::ValueEditorCommonSignals(QObject* parent)
    : QObject(parent) {}

babelwires::ValueEditorInterface* babelwires::ValueEditorInterface::getValueEditorInterface(QWidget* valueEditor) {
    QVariant property = valueEditor->property(ValueEditorInterface::s_propertyName);
    if (property.isValid()) {
        return qvariant_cast<ValueEditorInterface*>(property);
    }
    return nullptr;
}

const babelwires::ValueEditorInterface* babelwires::ValueEditorInterface::getValueEditorInterface(const QWidget* valueEditor) {
    QVariant property = valueEditor->property(ValueEditorInterface::s_propertyName);
    if (property.isValid()) {
        return qvariant_cast<ValueEditorInterface*>(property);
    }
    return nullptr;
}
