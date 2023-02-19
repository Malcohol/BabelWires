/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Hash/hash.hpp>

babelwires::TypeRef::TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_typeDescription(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef argument)
    : m_typeDescription(
          ConstructedTypeData{typeConstructorId, {std::move(argument)}}) {}

const babelwires::Type* babelwires::TypeRef::tryResolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type* operator()(std::nullptr_t) { return nullptr; }
        const babelwires::Type* operator()(PrimitiveTypeId typeId) { return m_typeSystem.tryGetPrimitiveType(typeId); }
        const babelwires::Type* operator()(const ConstructedTypeData& higherOrderData) {
            // TODO
            return nullptr;
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{ typeSystem };
    return std::visit(visitorMethods, m_typeDescription);
}

const babelwires::Type& babelwires::TypeRef::resolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type& operator()(std::nullptr_t) { throw TypeSystemException() << "A null type cannot be resolved."; }
        const babelwires::Type& operator()(PrimitiveTypeId typeId) { return m_typeSystem.getPrimitiveType(typeId); }
        const babelwires::Type& operator()(const ConstructedTypeData& higherOrderData) {
            // TODO
            throw TypeSystemException() << "TODO";
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{ typeSystem };
    return std::visit(visitorMethods, m_typeDescription);
}


std::string babelwires::TypeRef::serializeToString() const {
    struct VisitorMethods {
        std::string operator()(std::nullptr_t) { return "()"; }
        std::string operator()(PrimitiveTypeId typeId) { return typeId.serializeToString(); }
        std::string operator()(const ConstructedTypeData& higherOrderData) {
            std::ostringstream os;
            const auto& arguments = std::get<1>(higherOrderData);
            assert((arguments.size() > 0) && "0-arity type constructors are not permitted");
            os << std::get<0>(higherOrderData).serializeToString() << "<" << arguments[0].serializeToString();
            for (const auto& arg : Span{ arguments.cbegin() + 1, arguments.cend()} ) {
                os << arg.serializeToString();
            }
            os << ">";
            return os.str();
        }
    } visitorMethods;
    return std::visit(visitorMethods, m_typeDescription);
}

std::string babelwires::TypeRef::toString() const {
    // TODO
    return {};
}

babelwires::TypeRef babelwires::TypeRef::deserializeFromString(std::string_view str) {
    // TODO
    return {};
}

void babelwires::TypeRef::visitIdentifiers(IdentifierVisitor& visitor) {
    // Note: The visitor needs to access the actual stored data, so be careful to avoid copies.
    struct VisitorMethods {
        void operator()(std::nullptr_t) {}
        void operator()(PrimitiveTypeId& typeId) { m_visitor(typeId); }
        void operator()(ConstructedTypeData& higherOrderData) {
            m_visitor(std::get<0>(higherOrderData));
            Arguments& arguments = std::get<1>(higherOrderData);
            std::for_each(arguments.begin(), arguments.end(), [this](TypeRef& arg){ arg.visitIdentifiers(m_visitor); });
        }
        IdentifierVisitor& m_visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, m_typeDescription);
}

void babelwires::TypeRef::visitFilePaths(FilePathVisitor& visitor) {}

std::size_t babelwires::TypeRef::getHash() const {
    std::size_t hash = 0x123456789;
    // I wonder if the constructor of std::hash objects creates pointless overhead here?
    struct VisitorMethods {
        void operator()(std::nullptr_t) { hash::mixInto(m_currentHash, 0x11122233); }
        void operator()(const PrimitiveTypeId& typeId) { hash::mixInto(m_currentHash, typeId); }
        void operator()(const ConstructedTypeData& higherOrderData) {
            hash::mixInto(m_currentHash, std::get<0>(higherOrderData));
            const Arguments& arguments = std::get<1>(higherOrderData);
            std::for_each(arguments.cbegin(), arguments.cend(), [this](const TypeRef& arg){ hash::mixInto(m_currentHash, arg); });
        }
        std::size_t& m_currentHash;
    } visitorMethods{hash};
    std::visit(visitorMethods, m_typeDescription);
    return hash;
}

bool babelwires::TypeRef::operator==(const TypeRef& other) const {
    // TODO 
    return false;
}

bool babelwires::TypeRef::operator!=(const TypeRef& other) const {
    return !(*this == other);
}
