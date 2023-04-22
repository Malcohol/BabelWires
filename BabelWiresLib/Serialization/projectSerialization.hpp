/**
 * Functions for saving and loading a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Serialization/projectBundle.hpp>

#include <Common/DataContext/dataSerialization.hpp>

namespace babelwires {
    /// Isolates the codebase from the choice of format and other implementation details of serialization.
    class ProjectSerialization : public DataSerialization<ProjectBundle> {};
} // namespace babelwires
