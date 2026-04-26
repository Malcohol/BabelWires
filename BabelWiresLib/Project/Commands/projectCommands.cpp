/**
 * Instantiations of the command templates for project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/projectCommands.hpp>

template class BABELWIRESLIB_API babelwires::Command<babelwires::Project>;
template class BABELWIRESLIB_API babelwires::SimpleCommand<babelwires::Project>;
template class BABELWIRESLIB_API babelwires::CompoundCommand<babelwires::Project>;
