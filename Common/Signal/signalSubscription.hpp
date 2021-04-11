#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

namespace babelwires {

    struct SignalBase;

    /// Returned by Signal::subscribe.
    /// This must be kept alive in order to receive calls from the signal.
    class SignalSubscription {
      public:
        SignalSubscription() = default;
        SignalSubscription(SignalSubscription&&) = default;
        SignalSubscription& operator=(SignalSubscription&&) = default;

        SignalSubscription& operator=(const SignalSubscription&) = delete;
        SignalSubscription(const SignalSubscription&) = delete;

        /// Unsubscribes from the signal, if still subscribed.
        ~SignalSubscription();

        /// Unsubscribes from the signal.
        void unsubscribe();

      public:
        /// Shared between signal and a subscription.
        struct Handle {
            Handle(SignalBase* signal)
                : m_signal(signal) {}

            /// The signal to which this the subscription relates.
            SignalBase* m_signal;
        };

        SignalSubscription(std::shared_ptr<Handle> handle);

      private:
        std::shared_ptr<Handle> m_handle;
    };

    /// A non-template base interface for Signals, sufficient to unsubscribe subscriptions.
    struct SignalBase {
        virtual void unsubscribe(const SignalSubscription::Handle* handle) = 0;
    };

} // namespace babelwires