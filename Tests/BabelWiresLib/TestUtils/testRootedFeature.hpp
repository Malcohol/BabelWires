#pragma once

#include <BabelWiresLib/Features/rootFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace babelwires {
    class ProjectContext;
}

namespace testUtils {
    /// Convenience class to construct a T in a RootFeature, which is necessary
    /// if the contents of T need access to the ProjectContext.
    template<typename T>
    class RootedFeature {
      public:
        RootedFeature(babelwires::ProjectContext& projectContext)
            : m_rootFeature(projectContext)
            , m_feature(*m_rootFeature.addField(std::make_unique<T>(), testUtils::getTestRegisteredIdentifier("foo")))
        {
        }

        T& getFeature() { return m_feature; }

        babelwires::RootFeature m_rootFeature;
        T& m_feature;
    };
}
