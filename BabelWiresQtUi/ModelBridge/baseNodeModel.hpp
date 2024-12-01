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

    /// This is a common base for both the factories and the NodeNodeModel.
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

    /// Hack to work around the fact that nodeeditor expects registered items to be instances of the nodes they create.
    /// It seems to be a kind of prototype-based system. That might work for some systems, but doesn't make any sense
    /// for mine.
    class NodeFactoryBase {
      public:
        NodeFactoryBase(ProjectBridge* projectBridge)
            : m_projectBridge(projectBridge) {}

        virtual QString name() const = 0;

        std::unique_ptr<QtNodes::NodeDataModel> operator()() const {
            if (!m_queryHack) {
                m_queryHack = true;
                return std::make_unique<FactoryNameQuery>(*m_projectBridge, name());
            }
            return createNode();
        }

      protected:
        virtual std::unique_ptr<QtNodes::NodeDataModel> createNode() const = 0;
        ProjectBridge* m_projectBridge;

      private:
        mutable bool m_queryHack = false;
    };

} // namespace babelwires
