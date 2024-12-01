template<typename REGISTRY>
bool babelwires::ElementData::checkFactoryVersionCommon(const REGISTRY& reg,
                                                        UserLogger& userLogger,
                                                        LongId factoryIdentifier,
                                                        VersionNumber& thisVersion) {
    if (const auto* entry = reg.getEntryByIdentifier(factoryIdentifier)) {
        const VersionNumber registeredVersion = entry->getVersion();
        if (thisVersion == 0) {
            // Implicitly consider this version up-to-date.
            thisVersion = registeredVersion;
            return true;
        } else if (thisVersion < registeredVersion) {
            userLogger.logWarning() << "Data for the factory \"" << entry->getName() << "\" (" << factoryIdentifier
                                    << ") corresponds to an old version (" << thisVersion
                                    << "). The current version is " << registeredVersion
                                    << ". Some edits may not apply correctly.";
            return false;
        } else if (thisVersion > registeredVersion) {
            userLogger.logError() << "Data for the factory \"" << entry->getName() << "\" (" << factoryIdentifier
                                  << ") has an unknown version (" << thisVersion << "). The current version is "
                                  << registeredVersion << ". Some edits may not apply correctly.";
            return false;
        }
        return true;
    }
    return true;
}
