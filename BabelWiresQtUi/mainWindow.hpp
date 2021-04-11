/**
 * The MainWindow of the application.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/projectData.hpp"
#include "BabelWires/Project/uiPosition.hpp"

#include "Common/Signal/signalSubscription.hpp"

#include <QMainWindow>

#include <memory>

class QToolBar;

namespace babelwires {
    class UnifiedLog;
    struct UserLogger;
} // namespace babelwires

namespace babelwires {

    class ProjectBridge;
    struct ProjectData;
    class LogWindow;

    /// The MainWindow of the application.
    class MainWindow : public QMainWindow {
        Q_OBJECT

      public:
        MainWindow(ProjectBridge& projectBridge, UnifiedLog& log);
        ~MainWindow();

      private:
        void createActions();
        void createMenus();
        void createToolbars();
        void createDockWidgets(UnifiedLog& log);

      private slots:
        void openProject();
        void saveProject();
        void saveProjectAs();
        void newProject();
        void undo();
        void redo();
        void cut();
        void copy();
        void paste();
        void reloadAllSources();
        void saveAllTargets();
        void onNodeSelectionChanged(int numNodesSelected);
        void onClipboardChanged();

      private:
        /// Let the user save any unchanged changes before performing the operation.
        /// Returns true if the save was successful, or if the user is willing to
        /// discard the changes.
        bool maybeSave();

        /// Attempt to save the project, and issue a dialog if it fails.
        bool trySaveProject(const QString& filePath);

        void setInitialFilePath();
        void setCurrentFilePath(const QString& filePath);

        QString getFullFilePath() const;

        void onUndoStateChanged();

        /// Construct a ProjectData from the currently selected set of nodes.
        ProjectData getProjectDataFromSelection();

        /// Write the ProjectData as a string into the clipboard.
        void writeToClipboard(ProjectData projectData);

        QString getApplicationTitle() const;
        QString getProjectExtension() const;
        QString getDialogProjectFormat() const;
        QString getClipboardMimetype() const;

      public slots:
        void onShowMainToolbar(bool show);
        void onShowLogWindow(bool show);

      protected:
        void closeEvent(QCloseEvent* event) override;

      private:
        ProjectBridge& m_projectBridge;
        QToolBar* m_mainToolbar;
        UserLogger& m_userLogger;
        LogWindow* m_logWindow;

        std::unique_ptr<QAction> m_openProjectAction;
        std::unique_ptr<QAction> m_saveProjectAction;
        std::unique_ptr<QAction> m_saveProjectAsAction;
        std::unique_ptr<QAction> m_newProjectAction;
        std::unique_ptr<QAction> m_exitAction;

        std::unique_ptr<QAction> m_undoAction;
        std::unique_ptr<QAction> m_redoAction;
        std::unique_ptr<QAction> m_cutAction;
        std::unique_ptr<QAction> m_copyAction;
        std::unique_ptr<QAction> m_pasteAction;

        std::unique_ptr<QAction> m_showToolbarAction;
        std::unique_ptr<QAction> m_showLogWindow;

        std::unique_ptr<QAction> m_reloadAllSources;
        std::unique_ptr<QAction> m_saveAllTargets;

        QString m_currentProjectDir;
        QString m_currentProjectFileName;

        SignalSubscription m_undoStateChangedSubscription;
    };

} // namespace babelwires
