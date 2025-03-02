/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;
    struct UserLogger;
    class ValueTreeNode;
    class AccessModelScope;

    /// Base class of widgets which provide type-specific UIs for editing values.
    class ComplexValueEditor : public QWidget {
        Q_OBJECT

        public:
            ComplexValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const DataLocation& data);

            const DataLocation& getData() const;

            // TODO Why are these static?

            /// Convenience Function: Get the Feature referred to by the data, or assert.
            /// Note: This returns const because editors never modify features directly.
            static const ValueTreeNode& getValueTreeNode(AccessModelScope& scope, const DataLocation& data);

            /// Convenience Function: Get the Feature referred to by the data, or return nullptr.
            /// Note: This returns const because editors never modify features directly.
            static const ValueTreeNode* tryGetValueTreeNode(AccessModelScope& scope, const DataLocation& data);

            /// Convenience Function: Get the Feature referred to by the data, or throw a ModelException.
            /// Note: This returns const because editors never modify features directly.
            static const ValueTreeNode& getValueTreeNodeOrThrow(AccessModelScope& scope, const DataLocation& data);

        signals:
            void editorClosing();

        protected:
            ProjectBridge& getProjectBridge();
            UserLogger& getUserLogger();
            void closeEvent(QCloseEvent* event) override;

            /// Let the user apply any unchanged changes before performing the operation.
            /// Returns true if the save was successful, or if the user is willing to
            /// discard the changes.
            virtual bool maybeApplyToProject() = 0;

        private:
            /// Needed when making changes to the project.
            ProjectBridge& m_projectBridge;

            /// 
            UserLogger& m_userLogger;

            /// 
            std::unique_ptr<DataLocation> m_data;

    };

} // namespace babelwires
