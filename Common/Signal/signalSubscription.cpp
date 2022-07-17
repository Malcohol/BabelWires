/**
 * SignalSubscription manages the lifetime of a subscriber.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/Signal/signalSubscription.hpp>

babelwires::SignalSubscription::SignalSubscription(std::shared_ptr<Handle> handle)
    : m_handle(std::move(handle)) {}

babelwires::SignalSubscription::~SignalSubscription() {
    unsubscribe();
}

void babelwires::SignalSubscription::unsubscribe() {
    if (m_handle) {
        m_handle->m_signal->unsubscribe(m_handle.get());
        m_handle = nullptr;
    }
}
