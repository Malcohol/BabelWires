/**
 * A PathStep is a union of a ShortId and an ArrayIndex.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Path/pathStep.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/identifierVisitor.hpp>
#include <Common/exceptions.hpp>

#include <charconv>
#include <sstream>

void babelwires::PathStep::writeToStream(std::ostream& os) const {
    if (const ShortId* f = asField()) {
        os << *f;
    } else if (const ArrayIndex* index = asIndex()) {
        os << *index;
    } else {
        os << c_notAStepRepresentation;
    }
}

void babelwires::PathStep::writeToStreamReadable(std::ostream& os, const IdentifierRegistry& identifierRegistry) const {
    if (const ShortId* f = asField()) {
        os << identifierRegistry.getName(*f);
    } else if (const ArrayIndex* index = asIndex()) {
        os << "[" << *index << "]";
    } else {
        os << c_notAStepRepresentation;
    }
}

std::string babelwires::PathStep::serializeToString() const {
    if (const ShortId* f = asField()) {
        return f->serializeToString();
    } else if (const ArrayIndex* index = asIndex()) {
        return std::to_string(*index);
    } else {
        return c_notAStepRepresentation;
    }
}

babelwires::PathStep babelwires::PathStep::deserializeFromString(std::string_view str) {
    if (str.empty()) {
        throw ParseException() << "Could not parse a PathStep from an empty string.";
    }
    if (std::isdigit(str[0])) {
        ArrayIndex arrayIndex = 0;
        const char* strEnd = str.data() + str.size();
        std::from_chars_result result = std::from_chars(str.data(), strEnd, arrayIndex);
        if ((result.ec != std::errc()) || (result.ptr != strEnd)) {
            throw ParseException() << "Could not parse \"" << str << "\" as an array index";
        }
        return arrayIndex;
    } else if (str == c_notAStepRepresentation) {
        return PathStep();
    } else {
        return PathStep(ShortId::deserializeFromString(str));
    }
}

void babelwires::PathStep::visitIdentifiers(IdentifierVisitor& visitor) {
    if (ShortId* field = asField()) {
        visitor(*field);
    }
}
