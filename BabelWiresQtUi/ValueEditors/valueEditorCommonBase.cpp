/**
 * Common functionality for editors which allow the user to edit Values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>

#include <QWidget>

babelwires::ValueEditorCommonSignals::ValueEditorCommonSignals(QObject* parent)
    : QObject(parent) {}

babelwires::ValueEditorInterface& babelwires::ValueEditorInterface::getValueEditorInterface(QWidget* valueEditor) {
    QVariant property = valueEditor->property(ValueEditorInterface::s_propertyName);
    assert(property.isValid() && "The widget does not have a ValueEditorInterface property");
    auto interface = qvariant_cast<ValueEditorInterface*>(property);
    assert(interface && "The ValueEditorInterface property didn't have the correct type");
    return *interface;
}

const babelwires::ValueEditorInterface& babelwires::ValueEditorInterface::getValueEditorInterface(const QWidget* valueEditor) {
    QVariant property = valueEditor->property(ValueEditorInterface::s_propertyName);
    assert(property.isValid() && "The widget does not have a ValueEditorInterface property");
    auto interface = qvariant_cast<ValueEditorInterface*>(property);
    assert(interface && "The ValueEditorInterface property didn't have the correct type");
    return *interface;
}
