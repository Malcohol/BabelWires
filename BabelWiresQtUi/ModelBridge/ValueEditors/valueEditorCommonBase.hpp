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

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

namespace babelwires {

    /// An interface of common functionality for Value Editors.
    /// Since value editors have a common type of QWidget, this interface is not known by
    /// the compiler. Instead, it is found dynamically by querying a QObject propery.
    struct ValueEditorInterface {
        /// Typical implementations will set text to bold.
        virtual void setFeatureIsModified(bool isModified) = 0;

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

      private:
        /// Connects the FeatureModel::valuesMayHaveChanged signal to a slot which calls FeatureDelegate::setEditorData.
        QMetaObject::Connection m_valuesChangedConnection;
    };

} // namespace babelwires

/// Register pointers-to-ValueEditorInterface as something which can be stored in a QVariant.
Q_DECLARE_METATYPE(babelwires::ValueEditorInterface*);

#include "BabelWiresQtUi/ModelBridge/ValueEditors/valueEditorCommonBase_inl.hpp"
