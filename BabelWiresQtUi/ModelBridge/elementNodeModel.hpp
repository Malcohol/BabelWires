/**
 * ElementNodeModel is the NodeDataModels corresponding to FeatureElements.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class Feature;
    class FeatureElement;
    class ContentsCache;
    class Path;

    class FeatureView;
    class FeatureModel;
    class FeatureModelDelegate;
    class AccessModelScope;

    struct UiSize;

    class ElementNodeModel : public BaseNodeModel {
        Q_OBJECT

      public:
        ElementNodeModel(ProjectBridge& project, ElementId elementId);
        ~ElementNodeModel();

        virtual unsigned int nPorts(QtNodes::PortType portType) const override;
        virtual QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
        virtual QWidget* embeddedWidget() override;

        babelwires::ElementId getElementId() const;
        virtual QString caption() const override;

        const Path& getPathAtPort(AccessModelScope& scope, QtNodes::PortType portType,
                                         QtNodes::PortIndex portIndex) const;
        QtNodes::PortIndex getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                         const Path& path) const;

        ResizableAxes resizable() const override { return HorizontallyResizable; }

        void setSize(const UiSize& newSize);

        /// Return the model for the embedded widget.
        FeatureModel& getModel();

      public slots:
        void customContextMenuRequested(const QPoint& pos);

      protected:
        /// Set up the contents to match the state of element.
        void setContents(std::string label, ElementId elementId);

        static QtNodes::NodeDataType getDataTypeFromFeature(const Feature* f);

        const Feature* getInputFeature(AccessModelScope& scope, int portIndex) const;
        const Feature* getOutputFeature(AccessModelScope& scope, int portIndex) const;

      protected:
        ElementId m_elementId;

        FeatureView* m_view;
        FeatureModel* m_model;
        FeatureModelDelegate* m_delegate;

        int m_numRows = 0;
    };

} // namespace babelwires
