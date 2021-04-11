template <typename ENTRY, typename UNTYPED_REGISTRY>
babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::Registry(std::string registryName)
    : m_untypedRegistry(std::move(registryName)) {}

template <typename ENTRY, typename UNTYPED_REGISTRY>
void babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::addEntry(std::unique_ptr<ENTRY> newEntry) {
    m_untypedRegistry.addEntry(std::unique_ptr<RegistryEntry>(newEntry.release()));
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
const ENTRY* babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::getEntryByIdentifier(std::string_view identifier) const {
    assert(!identifier.empty() && "You can't query the registry with the empty string.");
    return static_cast<const ENTRY*>(m_untypedRegistry.getEntryByIdentifier(identifier));
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
const ENTRY& babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::getRegisteredEntry(std::string_view identifier) const {
    assert(!identifier.empty() && "You can't query the registry with the empty string.");
    return static_cast<const ENTRY&>(m_untypedRegistry.getRegisteredEntry(identifier));
}

template <typename ENTRY, typename UNTYPED_REGISTRY> class babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::Iterator {
  public:
    using ParentIterator = typename UNTYPED_REGISTRY::Iterator;
    Iterator() = default;
    Iterator(ParentIterator it)
        : m_iterator(it) {}
    const ENTRY& operator*() { return static_cast<const ENTRY&>(**m_iterator); }
    const ENTRY* operator->() { return static_cast<const ENTRY*>(m_iterator->get()); }
    Iterator& operator++() {
        ++m_iterator;
        return *this;
    }
    bool operator==(Iterator other) const { return m_iterator == other.m_iterator; }
    bool operator!=(Iterator other) const { return m_iterator != other.m_iterator; }

  private:
    ParentIterator m_iterator;
};

template <typename ENTRY, typename UNTYPED_REGISTRY>
typename babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::Iterator
babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::begin() const {
    return m_untypedRegistry.begin();
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
typename babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::Iterator
babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::end() const {
    return m_untypedRegistry.end();
}
