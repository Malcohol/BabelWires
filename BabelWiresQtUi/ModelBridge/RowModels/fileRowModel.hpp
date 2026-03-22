/**
 * The row model for a row corresponding to a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class FileNode;

    /// The file-model is special-cased to query the Node for the fileName.
    class BABELWIRESQTUI_API FileRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual QString getTooltip(ColumnType c) const override;

        virtual void
        getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut) const override;

      public:
        const FileNode& getFileElement() const;
    };

} // namespace babelwires
