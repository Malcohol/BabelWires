#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <cassert>

babelwires::TypeWidget::TypeWidget(QWidget* parent, const TypeSystem& typeSystem, const MapFeature::AllowedTypes& allowedTypeIds,
                                   TypeFlexibility flexibility)
    : QComboBox(parent)
    , m_hasBadItem(false) {
    m_defaultStyleSheet = styleSheet();
    m_badStyleSheet = styleSheet();

    // TODO This doesn't work as intended.
    m_badStyleSheet.append("\nQComboBox { background: red; }");

    std::vector<LongIdentifier> typeIds;
    for (auto typeId : allowedTypeIds.m_typeIds) {
        switch (flexibility) {
            case TypeFlexibility::strict:
                typeIds.emplace_back(typeId);
                break;
            case TypeFlexibility::allowSubtypes:
                typeSystem.addAllSubtypes(typeId, typeIds);
                break;
            case TypeFlexibility::allowSupertypes:
                typeSystem.addAllSupertypes(typeId, typeIds);
                break;
            case TypeFlexibility::allowRelatedTypes:
                typeSystem.addAllRelatedTypes(typeId, typeIds);
                break;
        }
    }
    TypeSystem::removeDuplicates(typeIds);

    std::vector<std::tuple<std::string, LongIdentifier>> sortedNames;
    sortedNames.reserve(typeIds.size());

    for (const auto& typeId : typeIds) {
        sortedNames.emplace_back(std::tuple{typeSystem.getEntryByIdentifier(typeId)->getName(), typeId});
    }
    std::sort(sortedNames.begin(), sortedNames.end());

    m_typeIds.swap(typeIds);
    m_typeIds.clear();
    for (const auto& name : sortedNames) {
        addItem(std::get<0>(name).c_str());
        m_typeIds.emplace_back(std::get<1>(name));
    }
    setTypeId(allowedTypeIds.getDefaultTypeId());

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TypeWidget::onCurrentIndexChanged);
}

babelwires::LongIdentifier babelwires::TypeWidget::getTypeId() const {
    assert(currentIndex() < m_typeIds.size());
    return m_typeIds[currentIndex()];
}

void babelwires::TypeWidget::setTypeId(LongIdentifier id) {
    auto it = std::find(m_typeIds.begin(), m_typeIds.end(), id);
    assert(it != m_typeIds.end());
    const int newIndex = it - m_typeIds.begin();
    setCurrentIndex(newIndex);
}

void babelwires::TypeWidget::addBadItemIfNotPresent(LongIdentifier id) {
    if (m_hasBadItem) {
        if (m_typeIds.back() == id) {
            return;
        } else {
            clearBadItem();
        }
    }
    m_typeIds.emplace_back(id);
    std::string typeName = IdentifierRegistry::read()->getName(id);
    addItem(typeName.c_str());
    const int newIndex = m_typeIds.size() - 1;
    const QModelIndex index = model()->index(newIndex, 0);
    model()->setData(index, QColor(Qt::red), Qt::BackgroundRole);
    m_hasBadItem = true;
}

int babelwires::TypeWidget::getBadItemIndex() const {
    assert(m_hasBadItem && "Bad item not set");
    return m_typeIds.size() - 1;
}

void babelwires::TypeWidget::clearBadItem() {
    assert(m_hasBadItem);
    removeItem(getBadItemIndex());
    m_typeIds.pop_back();
    m_hasBadItem = false;
}

void babelwires::TypeWidget::onCurrentIndexChanged(int index) {
    if (m_hasBadItem && (index == getBadItemIndex()))
    {
        setStyleSheet(m_badStyleSheet);
    }
    else {
        setStyleSheet(m_defaultStyleSheet);
    }
}

void babelwires::TypeWidget::removeBadItemIfPresent() {
    if (m_hasBadItem) {
        clearBadItem();
    }
}
