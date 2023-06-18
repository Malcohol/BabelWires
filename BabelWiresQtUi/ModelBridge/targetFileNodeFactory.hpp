/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class ProjectBridge;
    class TargetFileFormat;

    class TargetFileNodeFactory : public NodeFactoryBase {
      public:
        TargetFileNodeFactory(ProjectBridge* projectBridge, const TargetFileFormat* targetFileFormat);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        const TargetFileFormat* m_targetFileFormat;
    };

} // namespace babelwires
