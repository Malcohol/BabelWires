#pragma once

#include <Common/types.hpp>
#include <ostream>

namespace babelwires {
    class Feature;

    /// Write XML to the stream, corresponding to the contents of data, described by the property.
    /// There is no loader for this data. It's primarily intended for debugging.
    /// Note: I may deprecated this.
    void featureToXml(const Feature& feature, std::ostream& stream, bool writeTrackEvents = false);

} // namespace babelwires
