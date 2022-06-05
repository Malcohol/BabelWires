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
        enum class Variance { strict, covariant, contravariant };

        /// If a type identifier is not provided, all types are allowed.
        TypeWidget(QWidget* parent, const TypeSystem& typeSystem, std::optional<LongIdentifier> typeId, Variance variance);

        LongIdentifier getTypeId() const;
        void setTypeId(LongIdentifier id);

      private:
        std::vector<LongIdentifier> m_typeIds;
    };
}
