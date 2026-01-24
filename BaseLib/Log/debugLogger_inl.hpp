/**
 * An interface for submitting debug log messages, and a global function for issuing debug messages.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
inline babelwires::Log::MessageBuilder babelwires::logDebug() {
    assert(DebugLogger::g_debugLogger && "No debug logger has been set");
    return DebugLogger::g_debugLogger->logDebug();
}
