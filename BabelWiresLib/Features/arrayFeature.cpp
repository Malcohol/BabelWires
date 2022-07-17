/**
 * An ArrayFeature can contain a dynamically-sized sequence of features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/arrayFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Utilities/hash.hpp>

#include <limits>

babelwires::Feature* babelwires::ArrayFeature::addEntry(int indexOfNewEntry) {
    assert((m_entries.size() <= getSizeRange().m_max) && "Array size is out of range.");
    assert((indexOfNewEntry <= static_cast<int>(m_entries.size())) && "indexOfNewEntry is out of range");
    if (m_entries.size() == getSizeRange().m_max) {
        throw ModelException() << "Cannot add to an array which is already at its maximum size (" << m_entries.size()
                               << ")";
    }
    auto iterator = (indexOfNewEntry < 0) ? m_entries.end() : (m_entries.begin() + indexOfNewEntry);
    Entry newEntry{createNextEntry(), m_nextId};
    ++m_nextId;
    newEntry.m_feature->setOwner(this);
    newEntry.m_feature->setToDefault();
    iterator = m_entries.emplace(iterator, std::move(newEntry));
    setChanged(Changes::StructureChanged);
    return iterator->m_feature.get();
}

void babelwires::ArrayFeature::removeEntry(int indexOfEntryToRemove) {
    assert((m_entries.size() >= getSizeRange().m_min) && "Array size is out of range");
    assert((indexOfEntryToRemove < static_cast<int>(m_entries.size())) && "indexOfEntryToRemove is out of range");
    if (m_entries.size() == getSizeRange().m_min) {
        throw ModelException() << "Cannot remove from an array which is already at its minimum size ("
                               << m_entries.size() << ")";
    }
    const auto iterator =
        (indexOfEntryToRemove < 0) ? (m_entries.end() - 1) : (m_entries.begin() + indexOfEntryToRemove);
    m_entries.erase(iterator);
    setChanged(Changes::StructureChanged);
}

int babelwires::ArrayFeature::getNumFeatures() const {
    return m_entries.size();
}

babelwires::PathStep babelwires::ArrayFeature::getStepToChild(const Feature* child) const {
    for (ArrayIndex i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].m_feature.get() == child) {
            return PathStep(i);
        }
    }
    assert(!"No such child");
    return PathStep(0);
}

babelwires::Feature* babelwires::ArrayFeature::tryGetChildFromStep(const PathStep& step) {
    if (step.isIndex()) {
        if (step.getIndex() < m_entries.size()) {
            return m_entries[step.getIndex()].m_feature.get();
        }
    }
    return nullptr;
}

const babelwires::Feature* babelwires::ArrayFeature::tryGetChildFromStep(const PathStep& step) const {
    if (step.isIndex()) {
        if (step.getIndex() < m_entries.size()) {
            return m_entries[step.getIndex()].m_feature.get();
        }
    }
    return nullptr;
}

babelwires::Feature* babelwires::ArrayFeature::doGetFeature(int i) {
    return m_entries[i].m_feature.get();
}

const babelwires::Feature* babelwires::ArrayFeature::doGetFeature(int i) const {
    return m_entries[i].m_feature.get();
}

void babelwires::ArrayFeature::setSize(unsigned int newSize) {
    const auto sizeRange = getSizeRange();
    if ((newSize < sizeRange.m_min) || (newSize > sizeRange.m_max)) {
        throw ModelException() << "Array cannot be resized to size " << newSize;
    }

    while (getNumFeatures() < newSize) {
        addEntry();
    }

    while (getNumFeatures() > newSize) {
        removeEntry(-1);
    }
}

void babelwires::ArrayFeature::doSetToDefault() {
    unsigned int defaultSize = getSizeRange().m_min;
    while (getNumFeatures() > defaultSize) {
        removeEntry(-1);
    }

    setSubfeaturesToDefault();

    while (getNumFeatures() < defaultSize) {
        addEntry();
    }
}

void babelwires::ArrayFeature::doSetToDefaultNonRecursive() {
    unsigned int defaultSize = getSizeRange().m_min;
    while (getNumFeatures() > defaultSize) {
        removeEntry(-1);
    }

    while (getNumFeatures() < defaultSize) {
        addEntry();
    }
}

babelwires::Range<unsigned int> babelwires::ArrayFeature::getSizeRange() const {
    return doGetSizeRange();
}

babelwires::Range<unsigned int> babelwires::ArrayFeature::doGetSizeRange() const {
    return Range<unsigned int>{0, 16};
}

std::size_t babelwires::ArrayFeature::doGetHash() const {
    std::size_t hash = hash::mixtureOf(m_entries.size());
    for (const auto& e : m_entries) {
        // Shouldn't need to mix the entry IDs into the hash, since it doesn't affect the content.
        hash::mixInto(hash, e.m_feature->getHash());
    }
    return hash;
}

void babelwires::ArrayFeature::copyStructureFrom(const ArrayFeature& other) {
    // IDs are used to find the correct location for each existing element.
    std::unordered_map<EntryId, ArrayIndex> indexFromIdMap;
    for (unsigned int i = 0; i < m_entries.size(); ++i) {
        indexFromIdMap.insert(std::make_pair(m_entries[i].m_id, i));
    }

    decltype(m_entries) newEntries(other.m_entries.size());
    for (unsigned int i = 0; i < other.m_entries.size(); ++i) {
        const Entry& othersEntry = other.m_entries[i];
        Entry& newEntry = newEntries[i];
        const auto it = indexFromIdMap.find(othersEntry.m_id);
        if (it == indexFromIdMap.end()) {
            newEntry.m_feature = createNextEntry();
            newEntry.m_feature->setOwner(this);
            newEntry.m_id = othersEntry.m_id;
        } else {
            newEntry = std::move(m_entries[it->second]);
        }
    }
    m_entries.swap(newEntries);
    m_nextId = other.m_nextId;
}

const babelwires::ValueNames* babelwires::ArrayFeature::getEntryNames() const {
    return nullptr;
}
