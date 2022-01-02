/**
 * ValueEditors are type-specific UIs that allow values to be edited.
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
    struct ValueEditorData {
        ElementId m_elementId;
        FeaturePath m_pathToValue;

        std::size_t getHash() const;

        inline friend bool operator==(const ValueEditorData& a, const ValueEditorData& b) { 
            return (a.m_elementId == b.m_elementId) && (a.m_pathToValue == b.m_pathToValue);
        }

        friend std::ostream& operator<<(std::ostream& os, const ValueEditorData& data) {
            return os << data.m_elementId << ": " << data.m_pathToValue;
        }
    };

    /// Base class of widgets which provide type-specific UIs for editing values.
    class ValueEditor : public QWidget {
        public:
            ValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ValueEditorData& data);

            const ValueEditorData& getData() const;

        protected:
            ProjectBridge& getProjectBridge();

        private:
            /// Needed when making changes to the project.
            ProjectBridge& m_projectBridge;

            /// 
            UserLogger& m_userLogger;

            /// 
            ValueEditorData m_data;

    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ValueEditorData> {
        inline std::size_t operator()(const babelwires::ValueEditorData& data) const { return data.getHash(); }
    };
} // namespace std
