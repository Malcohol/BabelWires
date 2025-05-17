/**
 * The UiProjectContext extends the ProjectContext with some Ui specific data.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/projectContext.hpp>

#include <string>

namespace babelwires {
    class ValueModelRegistry;

    /// Values which give the application its identity.
    struct ApplicationIdentity
    {
        /// The name of the application ("BabelWires")
        std::string m_applicationTitle;
        /// The extension to use for projects (e.g. ".babelwires")
        std::string m_projectExtension;
    };

    struct UiProjectContext : ProjectContext {
        ValueModelRegistry& m_valueModelReg;
        ApplicationIdentity m_applicationIdentity;
    };

} // namespace babelwires
