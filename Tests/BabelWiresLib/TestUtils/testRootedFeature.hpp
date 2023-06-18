#pragma once

#include <BabelWiresLib/Features/rootFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace babelwires {
    struct ProjectContext;
}

namespace testUtils {
    /// Convenience class to construct a T in a RootFeature, which is necessary
    /// if the contents of T need access to the ProjectContext.
    template <typename T> class RootedFeature {
      public:
        template<typename... ARGS>
        RootedFeature(babelwires::ProjectContext& projectContext, ARGS&&... args)
            : m_rootFeature(projectContext)
            , m_feature(*m_rootFeature.addField(std::make_unique<T>(std::forward<ARGS>(args)...),
                                                testUtils::getTestRegisteredIdentifier("foo"))) {}

        babelwires::RootFeature& getRoot() { return m_rootFeature; }
        T& getFeature() { return m_feature; }

      private:
        babelwires::RootFeature m_rootFeature;
        T& m_feature;
    };
} // namespace testUtils
