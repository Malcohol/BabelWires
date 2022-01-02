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
    class UserLogger;

    /// Base class of widgets which provide type-specific UIs for editing values.
    class ComplexValueEditor : public QWidget {
        Q_OBJECT

        public:
            ComplexValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data);

            const ComplexValueEditorData& getData() const;

        signals:
            void editorClosing();

        protected:
            ProjectBridge& getProjectBridge();
            void closeEvent(QCloseEvent* event) override;

        private:
            /// Needed when making changes to the project.
            ProjectBridge& m_projectBridge;

            /// 
            UserLogger& m_userLogger;

            /// 
            ComplexValueEditorData m_data;

    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ComplexValueEditorData> {
        inline std::size_t operator()(const babelwires::ComplexValueEditorData& data) const { return data.getHash(); }
    };
} // namespace std
