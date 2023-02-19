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
#include <BabelWiresLib/Features/mapFeature.hpp>

#include <QComboBox>
#include <optional>

namespace babelwires {
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
          allowSupertypes,
          /// Allow the type, its subtypes and its supertypes to be selected
          allowRelatedTypes 
        };

        TypeWidget(QWidget* parent, const TypeSystem& typeSystem, const MapFeature::AllowedTypes& allowedTypeIds, TypeFlexibility flexibility);

        const TypeRef& getTypeId() const;
        void setTypeId(const TypeRef& id);

        /// Add an item for a type which should not be allowed.
        void addBadItemIfNotPresent(const TypeRef& id);

        /// If a badItem was added, remove it.
        void removeBadItemIfPresent();

      private:
        int getBadItemIndex() const;
        void clearBadItem();

        void onCurrentIndexChanged(int index);

      private:
        TypeSystem::TypeIdSet m_typeIds;
        bool m_hasBadItem;
        QString m_defaultStyleSheet;
        QString m_badStyleSheet;
    };
}
