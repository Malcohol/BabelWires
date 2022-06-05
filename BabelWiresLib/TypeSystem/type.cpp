#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::Type(LongIdentifier identifier, VersionNumber version, std::optional<LongIdentifier> parentTypeId)
    : RegistryEntry(identifier, version)
    , m_parentTypeId(parentTypeId)
{}

bool babelwires::Type::verifyParent(const Type& parentType) const {
    assert(false);
    return false;
}

std::optional<babelwires::LongIdentifier> babelwires::Type::getParentTypeId() const {
    return m_parentTypeId;
}

void babelwires::Type::addChild(LongIdentifier child) {
    m_children.emplace_back(child);
}

const std::vector<babelwires::LongIdentifier>& babelwires::Type::getChildren() const {
    return m_children;
}
