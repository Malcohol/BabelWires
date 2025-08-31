/**
 * The typeNameFormatter allows complex type names to be provided for type constructors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace babelwires {

    /// The naming syntax for TypeConstructors allows complex type names without the need for code.
    /// In particular, a deserialized TypeRef may still be given a user-meaningful name, even if it uses a
    /// TypeConstructor whose code is not present.
    /// * Type arguments are referenced using "{n}"
    /// * Value arguments are referenced using "[n]"
    /// * Curly and square brackets can be escaped using "{{", "}}", "[[" and "]]".
    /// * Optional arguments can be given replacements using {n?<replacement>} and [n?<replacement>].
    /// * Variadic type can be represented using format expressions of the form "{n|<operator chars>}"
    ///   This states that the types from n up should be written with the <operator chars> between them.
    /// * "}" and "]" used in optional or variadic expressions should be escaped with a "\" character.
    /// Example: "TC<{0?none}>([0])" with no type arguments and value argument 12 builds the string "TC<none>(12)".
    /// Example: "({0|,})" can build type names like "(Foo, Bar)".
    /// Example: "{0}+<[1|..]>+[0]" with type argument "Foo" and value arguments "a", "b", "c" and "d" builds the string "Foo+<b..c..d>+a".
    std::string typeNameFormatter(std::string_view format, const std::vector<std::string>& typeArguments,
                                  const std::vector<std::string>& valueArguments);
} // namespace babelwires
