/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class ProjectGraphModel;
    class ProcessorFactory;

    class ProcessorNodeFactory : public NodeFactoryBase {
      public:
        ProcessorNodeFactory(ProjectGraphModel* projectGraphModel, const ProcessorFactory* processorFactory);

        QString name() const override;
        std::unique_ptr<QtNodes::NodeDataModel> createNode() const;

      private:
        const ProcessorFactory* m_processorFactory;
    };

} // namespace babelwires
