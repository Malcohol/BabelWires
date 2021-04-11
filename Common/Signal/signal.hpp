/**
 * A Signal links callers and callees for a given function signature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Signal/signalSubscription.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace babelwires {

    /// Links callers and callees for a given function signature.
    template <typename... Params> class Signal : public SignalBase {
      public:
        /// Subscribe with something that can be cast to a std::function.
        /// The return value
        template <typename SLOT>[[nodiscard]] SignalSubscription subscribe(SLOT&& slot) {
            std::shared_ptr<SignalSubscription::Handle> handle = std::make_shared<SignalSubscription::Handle>(this);
            SignalSubscription subscription(handle);
            m_subscribers.emplace_back(Subscriber{std::forward<SLOT>(slot), std::move(handle)});
            return subscription;
        }

        /// Convenience method for subscribing with an object pointer and a pointer-to-member.
        template <typename O, typename... Args>[[nodiscard]] SignalSubscription subscribe(O* o, void (O::*m)(Args...)) {
            return subscribe([o, m](Args... args) { (o->*m)(args...); });
        }

        /// Call all subscribers with the given arguments.
        template <typename... Args> void fire(Args&&... args) const {
            for (auto&& subscriber : m_subscribers) {
                subscriber.m_slot(std::forward<Args>(args)...);
            }
        }

        ~Signal() {
            assert(m_subscribers.empty());
            // Protect subscribers from calling unsubscribe on freed memory.
            // for (auto& subscriber : m_subscribers)
            //{
            //    subscriber.m_handle->m_signal = nullptr;
            //}
        }

      protected:
        void unsubscribe(const SignalSubscription::Handle* handle) override {
            auto it = std::find_if(m_subscribers.begin(), m_subscribers.end(),
                                   [handle](Subscriber& subscriber) { return subscriber.m_handle.get() == handle; });
            assert((it != m_subscribers.end()) && "Unsubscribing when not subscribed");
            m_subscribers.erase(it);
        }

      private:
        typedef std::function<void(Params...)> Slot;

        struct Subscriber {
            Slot m_slot;
            std::shared_ptr<SignalSubscription::Handle> m_handle;
        };

        std::vector<Subscriber> m_subscribers;
    };

} // namespace babelwires
