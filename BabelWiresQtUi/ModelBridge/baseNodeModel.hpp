/**
 * BaseNodeModel is the base class shared by all the NodeDataModels in BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QtCore/QObject>
#include <nodes/NodeDataModel>

namespace babelwires {

    class ProjectBridge;

    /// This is a common base for both the factories and the element node model.
    class BaseNodeModel : public QtNodes::NodeDataModel {
        Q_OBJECT

      public:
        BaseNodeModel(ProjectBridge& project);
        virtual QString name() const override;
        virtual QString caption() const override;
        virtual unsigned int nPorts(QtNodes::PortType portType) const override;
        virtual QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
        virtual void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;
        virtual std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
        virtual QWidget* embeddedWidget() override;

      protected:
        ProjectBridge& m_projectBridge;
    };

    /// This hack allows the factories to provide an object to use
    /// for the name query, without actually creating their real
    /// target objects.
    class FactoryNameQuery : public BaseNodeModel {
      public:
        FactoryNameQuery(ProjectBridge& project, QString name)
            : BaseNodeModel(project)
            , m_name(std::move(name)) {}

        QString name() const { return m_name; }

      private:
        QString m_name;
    };

} // namespace babelwires
