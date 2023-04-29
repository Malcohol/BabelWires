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
    class RowModelRegistry;
    class ValueModelRegistry;

    /// Values which give a particular BabelWires application its identity.
    struct ApplicationIdentity
    {
        /// The name of the application (e.g. "FooEditor")
        std::string m_applicationTitle;
        /// The extension to use for projects (e.g. ".fooproj")
        std::string m_projectExtension;
    };

    struct UiProjectContext : ProjectContext {
        RowModelRegistry& m_rowModelReg;
        ValueModelRegistry& m_valueModelReg;
        ApplicationIdentity m_applicationIdentity;
    };

} // namespace babelwires
