/**
 * The UiProjectContext extends the ProjectContext with some Ui specific data.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>

#include <string>

namespace babelwires {
    class ValueModelRegistry;

    /// Values which give the application its identity.
    struct BABELWIRESQTUI_API ApplicationIdentity
    {
        /// The name of the application ("BabelWires")
        std::string m_applicationTitle;
        /// The extension to use for projects (e.g. ".babelwires")
        std::string m_projectExtension;
    };

    struct BABELWIRESQTUI_API UiProjectContext : ProjectContext {
        ValueModelRegistry& m_valueModelReg;
        ApplicationIdentity m_applicationIdentity;
    };

} // namespace babelwires
