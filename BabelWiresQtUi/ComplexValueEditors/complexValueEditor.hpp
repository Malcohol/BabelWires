/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <QWidget>

namespace babelwires {
    class ProjectBridge;
    class UserLogger;

    /// Data sufficient to describe the value the editor should be editing.
    struct ComplexValueEditorData {
        ElementId m_elementId;
        FeaturePath m_pathToValue;

        std::size_t getHash() const;

        inline friend bool operator==(const ComplexValueEditorData& a, const ComplexValueEditorData& b) { 
            return (a.m_elementId == b.m_elementId) && (a.m_pathToValue == b.m_pathToValue);
        }

        friend std::ostream& operator<<(std::ostream& os, const ComplexValueEditorData& data) {
            return os << data.m_elementId << ": " << data.m_pathToValue;
        }
    };

    /// Base class of widgets which provide type-specific UIs for editing values.
    class ComplexValueEditor : public QWidget {
        public:
            ComplexValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data);

            const ComplexValueEditorData& getData() const;

        protected:
            ProjectBridge& getProjectBridge();

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
