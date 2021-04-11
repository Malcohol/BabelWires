/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresQtUi/ModelBridge/baseNodeModel.hpp"

namespace babelwires {
    class ProjectBridge;
    class ProcessorFactory;

    class ProcessorNodeFactory {
      public:
        ProcessorNodeFactory(ProjectBridge* projectBridge, const ProcessorFactory* processorFactory);

        QString name() const;
        std::unique_ptr<QtNodes::NodeDataModel> operator()() const;

      private:
        ProjectBridge* m_projectBridge;
        const ProcessorFactory* m_processorFactory;
        mutable bool m_queryHack = false;
    };

} // namespace babelwires
