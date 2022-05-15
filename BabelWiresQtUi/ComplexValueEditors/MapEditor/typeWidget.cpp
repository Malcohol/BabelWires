#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

#include <cassert>

babelwires::TypeWidget::TypeWidget(QWidget* parent, ProjectBridge& projectBridge, TypeIdSet typeIds)
    : QComboBox(parent)
    , m_typeIds(std::move(typeIds)) {
    std::vector<std::string> typeNames;
    const UiProjectContext& context = projectBridge.getContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    for (const auto& e : m_typeIds) {
        typeNames.emplace_back(typeSystem.getEntryByIdentifier(e)->getName());
    }
    std::sort(typeNames.begin(), typeNames.end());
    for (const auto& name : typeNames) {
        addItem(name.c_str());
    }
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
