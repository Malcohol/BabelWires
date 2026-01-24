/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/typeWidget.hpp>

#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <cassert>
#include <unordered_set>

babelwires::TypeWidget::TypeWidget(QWidget* parent, const std::vector<TypeExp>& allowedTypeExps)
    : QComboBox(parent)
    , m_hasBadItem(false) {
    m_defaultStyleSheet = styleSheet();
    m_badStyleSheet = styleSheet();

    // TODO This doesn't work as intended.
    m_badStyleSheet.append("\nQComboBox { background: red; }");

    // Ensure uniqueness.
    std::unordered_set<TypeExp> typeExpSet;
    for (const auto& typeExp : allowedTypeExps) {
        typeExpSet.insert(typeExp);
    }

    std::vector<std::tuple<std::string, TypeExp>> sortedNames;
    sortedNames.reserve(typeExpSet.size());

    for (const auto& typeExp : typeExpSet) {
        sortedNames.emplace_back(std::tuple{typeExp.toString(), typeExp});
    }
    
    std::sort(sortedNames.begin(), sortedNames.end(), [](const auto& a, const auto& b) {
        return (std::get<0>(a) < std::get<0>(b)) ||
               ((std::get<0>(a) == std::get<0>(b)) &&
                (std::get<1>(a).toString() < std::get<1>(b).toString()));
    });

    m_typeExps.reserve(typeExpSet.size());
    m_typeExps.clear();
    for (const auto& name : sortedNames) {
        addItem(std::get<0>(name).c_str());
        m_typeExps.emplace_back(std::get<1>(name));
    }

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TypeWidget::onCurrentIndexChanged);
}

const babelwires::TypeExp& babelwires::TypeWidget::getTypeExp() const {
    assert(currentIndex() < m_typeExps.size());
    return m_typeExps[currentIndex()];
}

void babelwires::TypeWidget::setTypeExp(const TypeExp& id) {
    auto it = std::find(m_typeExps.begin(), m_typeExps.end(), id);
    assert(it != m_typeExps.end());
    const int newIndex = it - m_typeExps.begin();
    setCurrentIndex(newIndex);
}

void babelwires::TypeWidget::addBadItemIfNotPresent(const TypeExp& id) {
    if (m_hasBadItem) {
        if (m_typeExps.back() == id) {
            return;
        } else {
            clearBadItem();
        }
    }
    m_typeExps.emplace_back(id);
    std::string typeName = id.toString();
    addItem(typeName.c_str());
    const int newIndex = m_typeExps.size() - 1;
    const QModelIndex index = model()->index(newIndex, 0);
    model()->setData(index, QColor(Qt::red), Qt::BackgroundRole);
    m_hasBadItem = true;
}

int babelwires::TypeWidget::getBadItemIndex() const {
    assert(m_hasBadItem && "Bad item not set");
    return m_typeExps.size() - 1;
}

void babelwires::TypeWidget::clearBadItem() {
    assert(m_hasBadItem);
    removeItem(getBadItemIndex());
    m_typeExps.pop_back();
    m_hasBadItem = false;
}

void babelwires::TypeWidget::onCurrentIndexChanged(int index) {
    if (m_hasBadItem && (index == getBadItemIndex())) {
        setStyleSheet(m_badStyleSheet);
    } else {
        setStyleSheet(m_defaultStyleSheet);
    }
}

void babelwires::TypeWidget::removeBadItemIfPresent() {
    if (m_hasBadItem) {
        clearBadItem();
    }
}
