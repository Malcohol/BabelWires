/**
 * Common functionality for editors which allow the user to edit ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

class QModelIndex;
class QWidget;

#include <QMetaObject>
#include <QMetaType>
#include <QObject>
#include <QVariant>

namespace babelwires {

    /// Signals common for all the value editors.
    /// There's no common base class for the value editors, and QObjects may not be
    /// muliply-inherited, so instead we make them all own one of these and make
    /// it accessible through the ValueEditorInterface.
    class ValueEditorCommonSignals : public QObject {
        Q_OBJECT
      public:
        ValueEditorCommonSignals(QObject* parent);

      signals:
        void editorHasChanged(QWidget* editor);
    };

    /// An interface of common functionality for Value Editors.
    /// The common base type of value editors is QWidget, so this interface is not known by
    /// the compiler. Instead, it is found dynamically by querying a QObject propery.
    struct ValueEditorInterface {
        /// Typical implementations will set text to bold.
        virtual void setFeatureIsModified(bool isModified) = 0;

        /// Get somewhere to store the connection between the model and the delegate for this editor.
        virtual QMetaObject::Connection& getValuesChangedConnection() = 0;

        /// Get the object which carries the common signals.
        virtual ValueEditorCommonSignals* getValueEditorSignals() = 0;

        /// The interface is found in the QObject properties with this key.
        static constexpr char s_propertyName[] = "ValueEditorInterface";
    };

    /// For an editor T, this adds two features:
    /// 1. A lifetime managed connection from the model data to the contents of the editor
    ///    so if the former change, the latter updates.
    /// 2. Access to the ValueEditorInterface via a property.
    template <typename T> class ValueEditorCommonBase : public T, public ValueEditorInterface {
      public:
        /// The arguments as provided to createEditor.
        ValueEditorCommonBase(QWidget* parent, const QModelIndex& index);
        ~ValueEditorCommonBase();

        QMetaObject::Connection& getValuesChangedConnection() override;

        ValueEditorCommonSignals* getValueEditorSignals() override;

      protected:
        /// Stores the connection between the model and delegate for this editor.
        QMetaObject::Connection m_valuesChangedConnection;

        ValueEditorCommonSignals* m_signals;
    };

} // namespace babelwires

/// Register pointers-to-ValueEditorInterface as something which can be stored in a QVariant.
Q_DECLARE_METATYPE(babelwires::ValueEditorInterface*);

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase_inl.hpp>
