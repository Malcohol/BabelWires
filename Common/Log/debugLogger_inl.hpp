
inline babelwires::Log::MessageBuilder babelwires::logDebug() {
    assert(DebugLogger::g_debugLogger && "No debug logger has been set");
    return DebugLogger::g_debugLogger->logDebug();
}
