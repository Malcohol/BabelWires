/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/TypeSystem/typeIdSet.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <QComboBox>

namespace babelwires {
    class TypeSystem;
    class MapFeature;

    class TypeWidget : public QComboBox {
        Q_OBJECT
      public:
        TypeWidget(QWidget* parent, const TypeSystem& typeSystem, TypeIdSet typeIds);

        LongIdentifier getTypeId() const;
        void setTypeId(LongIdentifier id);

      private:
        TypeIdSet m_typeIds;
    };
}
