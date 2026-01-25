/**
 * A PathStep is a union of a ShortId and an ArrayIndex.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Path/pathStep.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Identifiers/identifierVisitor.hpp>
#include <BaseLib/exceptions.hpp>

#include <charconv>
#include <sstream>

// Try to ensure PathStep maintains the standard pattern for the IdentifierVisitor.
static_assert(babelwires::IdentifierVisitable<babelwires::PathStep>);

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

babelwires::ParseResult<babelwires::PathStep> babelwires::PathStep::deserializeFromString(std::string_view str) {
    if (str.empty()) {
        return Error() << "Could not parse a PathStep from an empty string.";
    }
    if (std::isdigit(str[0])) {
        ArrayIndex arrayIndex = 0;
        const char* strEnd = str.data() + str.size();
        std::from_chars_result result = std::from_chars(str.data(), strEnd, arrayIndex);
        if ((result.ec != std::errc()) || (result.ptr != strEnd)) {
            return Error() << "Could not parse \"" << str << "\" as an array index";
        }
        return arrayIndex;
    } else if (str == c_notAStepRepresentation) {
        return PathStep();
    } else {
        auto identifierResult = ShortId::deserializeFromString(str);
        if (!identifierResult) {
            return std::move(identifierResult.error());
        }
        return PathStep(*identifierResult);
    }
}

void babelwires::PathStep::visitIdentifiers(IdentifierVisitor& visitor) {
    if (ShortId* field = asField()) {
        visitor(*field);
    }
}
