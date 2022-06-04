/**
 * Manages the open set of ComplexValueEditors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp"
#include "BabelWiresQtUi/ComplexValueEditors/complexValueEditorFactory.hpp"

namespace babelwires {
    class ProjectBridge;
    struct UserLogger;

    class ComplexValueEditorManager : public QObject {
        Q_OBJECT

      public:
        void openEditorForValue(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger,
                                const ComplexValueEditorData& data);

        void closeAllValueEditors();

      public slots:
        void onValueEditorClose();

      private:
        ComplexValueEditorFactory m_valueEditorFactory;

        /// Use a vector to preserve the order of creation.
        /// It's very unlikely this will every be large, so no need for map.
        std::vector<ComplexValueEditor*> m_openValueEditors;
    };
} // namespace babelwires
