/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/exceptions.hpp>

namespace babelwires {
    class TypeSystemException : public ExceptionWithStream<TypeSystemException> {};
}
