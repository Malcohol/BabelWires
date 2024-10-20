/**
 * The row model for a row corresponding to a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>

namespace babelwires {

    class FileElement;

    /// The file-model is special-cased to query the feature element for the fileName.
    class FileRowModel : public RowModel {
      public:
        virtual QVariant getValueDisplayData() const override;

        virtual QString getTooltip() const override;

        virtual void
        getContextMenuActions(std::vector<FeatureContextMenuEntry>& actionsOut) const override;

      public:
        const FileElement& getFileElement() const;
    };

} // namespace babelwires
