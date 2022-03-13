#include "BabelWiresLib/Project/Modifiers/localModifier.hpp"

namespace testUtils {
    struct LocalTestModifier : babelwires::LocalModifier {
        LocalTestModifier(std::unique_ptr<babelwires::LocalModifierData> modifierData)
            : LocalModifier(std::move(modifierData)) {}
        LocalTestModifier(const LocalModifier& other)
            : LocalModifier(other) {}
        CLONEABLE(LocalTestModifier);

        void simulateFailure() { setFailed(babelwires::Modifier::State::ApplicationFailed, "synthetic failure"); }

        void simulateRecovery() { setSucceeded(); }
    };
} // namespace testUtils
