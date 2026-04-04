/**
 * ProjectVisibable defines an interface for classes storing project data, allowing them to expose certain data
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BaseLib/DataContext/dataVisitable.hpp>

namespace babelwires {
    struct BABELWIRESLIB_API ProjectVisitable : DataVisitable {};
}
