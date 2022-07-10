/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorData.hpp>

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;
    struct UserLogger;
    class ValueFeature;
    class AccessModelScope;

    /// Base class of widgets which provide type-specific UIs for editing values.
    class ComplexValueEditor : public QWidget {
        Q_OBJECT

        public:
            ComplexValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data);

            const ComplexValueEditorData& getData() const;

            // TODO Why are these static?

            /// Convenience Function: Get the ValueFeature referred to by the data, or assert.
            /// Note: This returns const because editors never modify features directly.
            static const ValueFeature& getValueFeature(AccessModelScope& scope, const ComplexValueEditorData& data);

            /// Convenience Function: Get the ValueFeature referred to by the data, or return nullptr.
            /// Note: This returns const because editors never modify features directly.
            static const ValueFeature* tryGetValueFeature(AccessModelScope& scope, const ComplexValueEditorData& data);

            /// Convenience Function: Get the ValueFeature referred to by the data, or throw a ModelException.
            /// Note: This returns const because editors never modify features directly.
            static const ValueFeature& getValueFeatureOrThrow(AccessModelScope& scope, const ComplexValueEditorData& data);

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
            ComplexValueEditorData m_data;

    };

} // namespace babelwires
