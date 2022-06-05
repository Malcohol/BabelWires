/**
 * Editor for editing map values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <QComboBox>
#include <optional>

namespace babelwires {
    class TypeSystem;
    class MapFeature;

    class TypeWidget : public QComboBox {
        Q_OBJECT
      public:
        enum class TypeFlexibility { 
          /// Only allow the given type to be selected.
          strict, 
          /// Allow the type and its subtypes to be selected
          allowSubtypes,
          /// Allow the type and its supertypes to be selected
          allowSupertypes 
        };

        /// If a type identifier is not provided, all types are allowed.
        TypeWidget(QWidget* parent, const TypeSystem& typeSystem, std::optional<LongIdentifier> typeId, TypeFlexibility flexibility);

        LongIdentifier getTypeId() const;
        void setTypeId(LongIdentifier id);

      private:
        std::vector<LongIdentifier> m_typeIds;
    };
}
