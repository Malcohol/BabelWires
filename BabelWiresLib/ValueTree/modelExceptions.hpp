/**
 * The exception classes which can be fired when interacting with the model.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BaseLib/exceptions.hpp>

namespace babelwires {
    /// The base exception class which can be fired when interacting with the model.
    class BABELWIRESLIB_API ModelException : public ExceptionWithStream<ModelException> {};

} // namespace babelwires
