/**
 * A ComplexValueEditorFactory creates the appropriate ComplexValueEditor for a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class UnifiedLog;
    struct UserLogger;
    class ProjectBridge;
 
    /// Constructs the correct type of editor to open.
    class ComplexValueEditorFactory {
      public:
        /// This will throw a ModelException if it does not make sense to open an editor for the value at data.
        ComplexValueEditor* createEditor(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ProjectDataLocation& data);
    };
}
