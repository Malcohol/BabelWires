/**
 * Framework-agnostic entry point for creating and running the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/uiMain.hpp"

#include <QtWidgets/QApplication>

#include "BabelWiresLib/Commands/commandManager.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/mainWindow.hpp"
#include "BabelWiresQtUi/uiProjectContext.hpp"

#include "Common/Log/unifiedLog.hpp"

struct babelwires::Ui::Impl {
    Impl(int& argc, char** argv, babelwires::UiProjectContext& projectContext, UnifiedLog& log)
        : m_app(argc, argv)
        , m_projectContext(projectContext)
        , m_log(log) {}

    void runMainLoop() {
        Project project(m_projectContext, m_log);
        CommandManager commandManager(project, m_log);
        ProjectBridge projectBridge(project, commandManager, m_projectContext);
        MainWindow mainWidget(projectBridge, m_log);
        projectBridge.setMainWindow(&mainWidget);

        m_app.exec();
    }

    QApplication m_app;
    babelwires::UiProjectContext& m_projectContext;
    UnifiedLog& m_log;
};

babelwires::Ui::Ui(int& argc, char** argv, babelwires::UiProjectContext& projectContext, UnifiedLog& log) {
    m_impl = std::make_unique<Impl>(argc, argv, projectContext, log);
}

babelwires::Ui::~Ui() = default;

void babelwires::Ui::runMainLoop() {
    m_impl->runMainLoop();
}
