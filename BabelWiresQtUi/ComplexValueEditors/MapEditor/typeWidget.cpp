#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <cassert>

babelwires::TypeWidget::TypeWidget(QWidget* parent, const TypeSystem& typeSystem, const MapFeature::AllowedTypes& allowedTypeRefs)
    : QComboBox(parent)
    , m_hasBadItem(false) {
    m_defaultStyleSheet = styleSheet();
    m_badStyleSheet = styleSheet();

    // TODO This doesn't work as intended.
    m_badStyleSheet.append("\nQComboBox { background: red; }");

    std::vector<TypeRef> typeRefs = allowedTypeRefs.m_typeRefs;
    std::sort(typeRefs.begin(), typeRefs.end());
    typeRefs.erase(std::unique(typeRefs.begin(), typeRefs.end()), typeRefs.end());

    std::vector<std::tuple<std::string, TypeRef>> sortedNames;
    sortedNames.reserve(typeRefs.size());

    for (const auto& typeRef : typeRefs) {
        const Type& type = typeRef.resolve(typeSystem);
        if (!type.isAbstract()) {
            sortedNames.emplace_back(std::tuple{type.getName(), typeRef});
        }
    }
    std::sort(sortedNames.begin(), sortedNames.end());

    m_typeRefs.swap(typeRefs);
    m_typeRefs.clear();
    for (const auto& name : sortedNames) {
        addItem(std::get<0>(name).c_str());
        m_typeRefs.emplace_back(std::get<1>(name));
    }

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TypeWidget::onCurrentIndexChanged);
}

const babelwires::TypeRef& babelwires::TypeWidget::getTypeRef() const {
    assert(currentIndex() < m_typeRefs.size());
    return m_typeRefs[currentIndex()];
}

void babelwires::TypeWidget::setTypeRef(const TypeRef& id) {
    auto it = std::find(m_typeRefs.begin(), m_typeRefs.end(), id);
    assert(it != m_typeRefs.end());
    const int newIndex = it - m_typeRefs.begin();
    setCurrentIndex(newIndex);
}

void babelwires::TypeWidget::addBadItemIfNotPresent(const TypeRef& id) {
    if (m_hasBadItem) {
        if (m_typeRefs.back() == id) {
            return;
        } else {
            clearBadItem();
        }
    }
    m_typeRefs.emplace_back(id);
    std::string typeName = id.toString();
    addItem(typeName.c_str());
    const int newIndex = m_typeRefs.size() - 1;
    const QModelIndex index = model()->index(newIndex, 0);
    model()->setData(index, QColor(Qt::red), Qt::BackgroundRole);
    m_hasBadItem = true;
}

int babelwires::TypeWidget::getBadItemIndex() const {
    assert(m_hasBadItem && "Bad item not set");
    return m_typeRefs.size() - 1;
}

void babelwires::TypeWidget::clearBadItem() {
    assert(m_hasBadItem);
    removeItem(getBadItemIndex());
    m_typeRefs.pop_back();
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
