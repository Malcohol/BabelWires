#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

babelwires::TypeWidget::TypeWidget(QWidget* parent, ProjectBridge& projectBridge, const TypeIdSet& typeIds)
    : QComboBox(parent) {
    std::vector<std::string> typeNames;
    const UiProjectContext& context = projectBridge.getContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    for (const auto& e : typeIds) {
        typeNames.emplace_back(typeSystem.getEntryByIdentifier(e)->getName());
    }
    std::sort(typeNames.begin(), typeNames.end());
    for (const auto& name : typeNames) {
        addItem(name.c_str());
    }
}