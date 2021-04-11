/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/projectData.hpp"

#include "Common/Serialization/serializable.hpp"

#include <map>

namespace babelwires {

    /// A ProjectBundle carries ProjectData along with metadata sufficient to version it (i.e. load it into
    /// a system which has changed since the data was saved).
    struct ProjectBundle : Serializable {
        SERIALIZABLE(ProjectBundle, "project", void, 1);
        ProjectBundle();
        ProjectBundle(const ProjectBundle&) = delete;
        ProjectBundle(ProjectBundle&&) = default;

        /// Construct a bundle whose projectData corresponds to the stored fieldNameRegistry.
        /// The data is modified to make the bundle independent of the current system.
        ProjectBundle(ProjectData&& projectData);

        ProjectBundle& operator=(const ProjectBundle&) = delete;
        ProjectBundle& operator=(ProjectBundle&&) = default;

        /// Returns the contained projectData as one which corresponds to the current system.
        /// The versions of the factories in the stored ProjectData from the factory meta-data are updated.
        /// This object is invalidated after calling this.
        /// NOTE: No versioning is done! All that currently happens is that warnings and errors are issued.
        ProjectData resolveAgainstCurrentContext(const ProjectContext& context, UserLogger& userLogger) &&;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// The data.
        ProjectData m_projectData;

        /// Field meta-data.
        FieldNameRegistry m_fieldNameRegistry;

        struct Metadata {
            /// Factory meta-data
            std::map<std::string, VersionNumber> m_factoryMetadata;
        };

        Metadata m_metadata;
    };

} // namespace babelwires
