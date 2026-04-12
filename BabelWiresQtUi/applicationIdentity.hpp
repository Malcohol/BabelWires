/**
 * Values which give the application its identity.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BaseLib/Context/context.hpp>

#include <string>

namespace babelwires {

    /// Values which give the application its identity.
    /// NOTE: The application version is the same as the codebase-defined version (obtained with Version::getProjectVersion()).
    struct BABELWIRESQTUI_API ApplicationIdentity
    {
        /// The name of the application ("BabelWires")
        std::string m_applicationTitle;
        /// The extension to use for projects (e.g. ".babelwires")
        std::string m_projectExtension;
    };

} // namespace babelwires
