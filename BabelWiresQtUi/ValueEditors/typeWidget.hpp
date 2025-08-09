/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <QComboBox>
#include <optional>

namespace babelwires {
    class TypeWidget : public QComboBox {
        Q_OBJECT
      public:
        TypeWidget(QWidget* parent, const std::vector<TypeRef>& allowedTypeRefs);

        const TypeRef& getTypeRef() const;
        void setTypeRef(const TypeRef& id);

        /// Add an item for a type which should not be allowed.
        void addBadItemIfNotPresent(const TypeRef& id);

        /// If a badItem was added, remove it.
        void removeBadItemIfPresent();

      private:
        int getBadItemIndex() const;
        void clearBadItem();

        void onCurrentIndexChanged(int index);

      private:
        std::vector<TypeRef> m_typeRefs;
        bool m_hasBadItem;
        QString m_defaultStyleSheet;
        QString m_badStyleSheet;
    };
}
