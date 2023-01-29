/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename ENTRY, typename UNTYPED_REGISTRY>
babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::Registry(std::string registryName)
    : m_untypedRegistry(std::move(registryName)) {}

template <typename ENTRY, typename UNTYPED_REGISTRY>
template <typename ENTRY_SUBTYPE, std::enable_if_t<std::is_base_of_v<ENTRY, ENTRY_SUBTYPE>, std::nullptr_t>>
ENTRY_SUBTYPE* babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::addEntry(std::unique_ptr<ENTRY_SUBTYPE> newEntry) {
    return static_cast<ENTRY_SUBTYPE*>(m_untypedRegistry.addEntry(std::unique_ptr<RegistryEntry>(newEntry.release())));
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
const ENTRY* babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::getEntryByIdentifier(const LongIdentifier& identifier) const {
    return static_cast<const ENTRY*>(m_untypedRegistry.getEntryByIdentifier(identifier));
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
const ENTRY& babelwires::Registry<ENTRY, UNTYPED_REGISTRY>::getRegisteredEntry(const LongIdentifier& identifier) const {
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
