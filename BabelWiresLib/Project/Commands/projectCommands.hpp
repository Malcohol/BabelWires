/**
 * Instantiations of the command templates for project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class Project;

    extern template class BABELWIRESLIB_EXTERNING_INSTANTIATION Command<Project>;
    extern template class BABELWIRESLIB_EXTERNING_INSTANTIATION SimpleCommand<Project>;
    extern template class BABELWIRESLIB_EXTERNING_INSTANTIATION CompoundCommand<Project>;

} // namespace babelwires
