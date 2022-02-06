#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

babelwires::TypeWidget::TypeWidget(QWidget* parent, ProjectBridge& projectBridge, const MapFeature::TypeSet& typeIds)
    : QComboBox(parent) {
        std::vector<std::string> typeNames;
        const UiProjectContext& context = projectBridge.getContext();
        const EnumRegistry& reg = context.m_enumRegistry;
        for (const auto& e : typeIds) {
            typeNames.emplace_back(reg.getEntryByIdentifier(e)->getName());
        }
        std::sort(typeNames.begin(), typeNames.end());
        for(const auto& name : typeNames) {
            addItem(name.c_str());
        }
    }