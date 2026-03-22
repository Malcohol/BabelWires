/**
 * A ComplexValueEditorFactory creates the appropriate ComplexValueEditor for a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BaseLib/Result/result.hpp>

#include <BabelWiresLib/Project/projectIds.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

namespace babelwires {
    class UnifiedLog;
    struct UserLogger;
    class ProjectGraphModel;
 
    /// Constructs the correct type of editor to open.
    class BABELWIRESQTUI_API ComplexValueEditorFactory {
      public:
        /// This will return an error if it does not make sense to open an editor for the value at data.
        ResultT<ComplexValueEditor*> createEditor(QWidget* parent, ProjectGraphModel& projectGraphModel, UserLogger& userLogger, const ProjectDataLocation& data);
    };
}
