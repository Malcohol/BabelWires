/**
 * A PathStep is a union of a ShortId and an ArrayIndex.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/Path/pathStep.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/exceptions.hpp>

#include <charconv>
#include <sstream>

void babelwires::PathStep::writeToStream(std::ostream& os) const {
    if (const ShortId* f = asField()) {
        os << *f;
    } else {
        os << getIndex();
    }
}

void babelwires::PathStep::writeToStreamReadable(std::ostream& os, const IdentifierRegistry& identifierRegistry) const {
    if (const ShortId* f = asField()) {
        os << identifierRegistry.getName(*f);
    } else {
        os << "[" << getIndex() << "]";
    }
}

std::string babelwires::PathStep::serializeToString() const {
    if (const ShortId* f = asField()) {
        return f->serializeToString();
    } else {
        return std::to_string(getIndex());
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
    } else {
        return PathStep(ShortId::deserializeFromString(str));
    }
}
