/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class ProjectBridge;
    class SourceFileFormat2;

    /// The factory which creates nodes for source files.
    class SourceFileNodeFactory : public NodeFactoryBase {
      public:
        SourceFileNodeFactory(ProjectBridge* projectBridge, const SourceFileFormat2* sourceFileFormat);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        const SourceFileFormat2* m_sourceFileFormat;
    };

} // namespace babelwires
