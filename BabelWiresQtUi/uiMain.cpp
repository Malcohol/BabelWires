/**
 * Framework-agnostic entry point for creating and running the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/uiMain.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/mainWindow.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Log/unifiedLog.hpp>

#include <QtWidgets/QApplication>

struct babelwires::Ui::Impl {
    Impl(int& argc, char** argv, babelwires::UiProjectContext& projectContext, UnifiedLog& log)
        : m_app(argc, argv)
        , m_projectContext(projectContext)
        , m_log(log) {}

    void runMainLoop() {
        Project project(m_projectContext, m_log);
        CommandManager commandManager(project, m_log);
        ProjectGraphModel projectGraphModel(project, commandManager, m_projectContext);
        MainWindow mainWidget(projectGraphModel, m_log);

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
