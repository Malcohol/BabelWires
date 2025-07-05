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
    /// TypeConstructor whose code is not present. The format syntax allows type arguments to be referenced using "{n}"
    /// and value arguments to be referenced using "[n]". Curly and square brackets can be escaped using "{{", "}}",
    /// "[[" and "]]". Variadic type constructors can be represented too. A format expression of the form "{n|<operator
    /// chars>}" describes how the types from n and up will be written with the <operator chars> between them. "}" and
    /// "]" used in operators by escaping them with a "\" character. So "({0|,})" can build type names like 
    /// "(Foo, Bar)". The (ridiculous) type constructor name "<{0}>+<[1|...]>+[0]" with type argument "Foo" and value
    /// arguments "a", "b" and "c" will build the string "<Foo>+<b...c>+a".
    std::string typeNameFormatter(std::string_view format, const std::vector<std::string>& typeArguments,
                                  const std::vector<std::string>& valueArguments);
} // namespace babelwires
