/**
 * A spinBox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <BabelWiresQtUi/ValueModels/valueModelDispatcher.hpp>

#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

#include <QWidget>

namespace babelwires {
    class TupleType;

    ///
    class TupleValueEditor : public ValueEditorCommonBase<QWidget> {
        Q_OBJECT
      public:
        ///
        TupleValueEditor(QWidget* parent, const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem,
                         const TupleType& type, const TupleValue& value);

        TupleValue getEditorData() const;

        void setEditorData(const TupleValue& tupleValue);

        /// Set the text to bold.
        void setFeatureIsModified(bool isModified) override;

        const TupleType& getType() const;

      private:
        void updateValueFromComponentEditor(unsigned int i);

      public:
        const TupleType& m_tupleType;

        struct PerComponentData {
            ValueModelDispatcher m_valueModel;
            QWidget* m_valueEditor;
            EditableValueHolder m_value;
        };
        std::vector<PerComponentData> m_perComponentData;
    };

} // namespace babelwires
