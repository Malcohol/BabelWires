/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename ENTRY>
babelwires::Registry<ENTRY>::Registry(std::string registryName)
    : m_untypedRegistry(std::move(registryName)) {}

template <typename ENTRY> std::string babelwires::Registry<ENTRY>::getRegistryName() const {
    return m_untypedRegistry.m_registryName;
}

template <typename ENTRY>
template <typename ENTRY_SUBTYPE, std::enable_if_t<std::is_base_of_v<ENTRY, ENTRY_SUBTYPE>, std::nullptr_t>>
ENTRY_SUBTYPE* babelwires::Registry<ENTRY>::addEntry(std::unique_ptr<ENTRY_SUBTYPE> newEntry) {
    assert(newEntry && "Registered entries must be non-null");
    onEntryRegistered(*newEntry);
    return static_cast<ENTRY_SUBTYPE*>(m_untypedRegistry.addEntry(std::unique_ptr<RegistryEntry>(newEntry.release())));
}

template <typename ENTRY>
template <typename ENTRY_SUBTYPE, typename... ARGS,
          std::enable_if_t<std::is_base_of_v<ENTRY, ENTRY_SUBTYPE>, std::nullptr_t>>
ENTRY_SUBTYPE* babelwires::Registry<ENTRY>::addEntry(ARGS&&... args) {
    return addEntry(std::make_unique<ENTRY_SUBTYPE>(std::forward<ARGS>(args)...));
}

template <typename ENTRY>
const ENTRY* babelwires::Registry<ENTRY>::getEntryByIdentifier(const LongId& identifier) const {
    return static_cast<const ENTRY*>(m_untypedRegistry.getEntryByIdentifier(identifier));
}

template <typename ENTRY> const ENTRY& babelwires::Registry<ENTRY>::getRegisteredEntry(const LongId& identifier) const {
    return static_cast<const ENTRY&>(m_untypedRegistry.getRegisteredEntry(identifier));
}

template <typename ENTRY> class babelwires::Registry<ENTRY>::Iterator {
  public:
    using ParentIterator = typename UntypedRegistry::Iterator;
    Iterator() = default;
    Iterator(ParentIterator it)
        : m_iterator(it) {}
    const ENTRY& operator*() { return static_cast<const ENTRY&>(*m_iterator->second.get()); }
    const ENTRY* operator->() { return static_cast<const ENTRY*>(m_iterator->second.get()); }
    Iterator& operator++() {
        ++m_iterator;
        return *this;
    }
    bool operator==(Iterator other) const { return m_iterator == other.m_iterator; }
    bool operator!=(Iterator other) const { return m_iterator != other.m_iterator; }

  private:
    ParentIterator m_iterator;
};

template <typename ENTRY> typename babelwires::Registry<ENTRY>::Iterator babelwires::Registry<ENTRY>::begin() const {
    return m_untypedRegistry.begin();
}

template <typename ENTRY> typename babelwires::Registry<ENTRY>::Iterator babelwires::Registry<ENTRY>::end() const {
    return m_untypedRegistry.end();
}
