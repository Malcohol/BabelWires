/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

babelwires::TypeRef::TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_typeDescription(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId unaryTypeConstructorId, TypeRef argument)
    : m_typeDescription(
          HigherOrderTypeData<1>{unaryTypeConstructorId, std::unique_ptr<Arguments<1>>(new Arguments<1>{std::move(argument)})}) {}

babelwires::TypeRef::Storage babelwires::TypeRef::deepCopy(const Storage& otherStorage) {
    struct VisitorMethods {
        babelwires::TypeRef::Storage operator()(std::nullptr_t) { return nullptr; }
        babelwires::TypeRef::Storage operator()(PrimitiveTypeId typeId) { return typeId; }
        babelwires::TypeRef::Storage operator()(const HigherOrderTypeData<1>& higherOrderData) { 
            return HigherOrderTypeData<1>{std::get<0>(higherOrderData), std::unique_ptr<Arguments<1>>(new Arguments<1>{
                                                                            (*std::get<1>(higherOrderData))[0]})};
        }
    } visitorMethods;
    return std::visit(visitorMethods, otherStorage);
}

babelwires::TypeRef::TypeRef(const TypeRef& other)
    : m_typeDescription(deepCopy(other.m_typeDescription)) {}

std::string babelwires::TypeRef::serializeToString() const {
    struct VisitorMethods {
        std::string operator()(std::nullptr_t) { return "()"; }
        std::string operator()(PrimitiveTypeId typeId) { return typeId.serializeToString(); }
        std::string operator()(const HigherOrderTypeData<1>& higherOrderData) {
            std::ostringstream os;
            os << std::get<0>(higherOrderData).serializeToString() << "<"
               << (*std::get<1>(higherOrderData))[0].serializeToString() << ">";
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
    struct VisitorMethods {
        void operator()(std::nullptr_t) {}
        void operator()(PrimitiveTypeId typeId) { m_visitor(typeId); }
        void operator()(const HigherOrderTypeData<1>& higherOrderData) {
            m_visitor(std::get<0>(higherOrderData));
            (*std::get<1>(higherOrderData))[0].visitIdentifiers(m_visitor);
        }
        IdentifierVisitor& m_visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, m_typeDescription);
}

void babelwires::TypeRef::visitFilePaths(FilePathVisitor& visitor) {}
