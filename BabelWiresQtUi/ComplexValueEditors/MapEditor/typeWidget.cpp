#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/typeWidget.hpp>

#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <cassert>

babelwires::TypeWidget::TypeWidget(QWidget* parent, const TypeSystem& typeSystem, std::optional<LongIdentifier> typeId,
                                   TypeFlexibility flexibility)
    : QComboBox(parent) {
    std::vector<LongIdentifier> typeIds;
    if (typeId.has_value()) {
        switch (flexibility) {
            case TypeFlexibility::strict:
                typeIds.emplace_back(*typeId);
                break;
            case TypeFlexibility::allowSubtypes:
                typeSystem.addAllSubtypes(*typeId, typeIds);
                break;
            case TypeFlexibility::allowSupertypes:
                typeSystem.addAllSupertypes(*typeId, typeIds);
                break;
        }
    } else {
        // TODO All types.
    }

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
