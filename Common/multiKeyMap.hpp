/**
 * The MultiKeyMap is a simple implementation of a map which can be indexed by more than one key.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cassert>
#include <tuple>
#include <unordered_map>

namespace babelwires {

    // A simple implementation of a map which can be indexed by more than one key.
    // Note: There is an asymmetry. Look-ups on Key1 require two map look-ups.
    template <typename Key0, typename Key1, typename Value> class MultiKeyMap {
      public:
        /// Returns true if there the entry for (k0, k1) is new, otherwise false.
        bool insert_or_assign(Key0 k0, Key1 k1, Value v) {
            assert(((m_map0.find(k0) == m_map0.end()) || (std::get<0>(m_map0.find(k0)->second) == k1)) &&
                   "k0 is already paired with a different k1");
            assert(((m_map1.find(k1) == m_map1.end()) || (m_map1.find(k1)->second == k0)) &&
                   "k1 is already paired with a different k0");
            auto result0 = m_map0.insert_or_assign(k0, std::tuple<Key1, Value>(k1, std::move(v)));
            m_map1.insert_or_assign(std::move(k1), std::move(k0));
            return result0.second;
        }

        class iterator {
          public:
            const Key0& getKey0() const { return m_it0->first; }

            const Key1& getKey1() const { return std::get<0>(m_it0->second); }

            const Value& getValue() const { return std::get<1>(m_it0->second); }

            bool operator==(const iterator& other) const { return m_it0 == other.m_it0; }
            bool operator!=(const iterator& other) const { return m_it0 != other.m_it0; }

          private:
            using iterator0 = typename decltype(MultiKeyMap::m_map0)::iterator;

            friend MultiKeyMap;
            iterator(iterator0 it0)
                : m_it0(std::move(it0)) {}

            iterator0 m_it0;
        };

        iterator find0(const Key0& k0) { return iterator{m_map0.find(k0)}; }

        iterator find1(const Key1& k1) {
            auto it1 = m_map1.find(k1);
            if (it1 != m_map1.end()) {
                const Key0& k0 = it1->second;
                auto it0 = m_map0.find(k0);
                assert((it0 != m_map0.end()) && "k0 of k1 not in map0");
                assert((std::get<0>(it0->second)) == k1 && "Keys don't match");
                return iterator{std::move(it0)};
            }
            return end();
        }

        iterator end() { return iterator{m_map0.end()}; }

        bool erase0(const Key0& k0) {
            auto it0 = m_map0.find(k0);
            if (it0 == m_map0.end()) {
                return false;
            }
            const Key1& k1 = std::get<0>(it0->second);
            auto it1 = m_map1.find(k1);
            assert((it1 != m_map1.end()) && "k1 of k0 not in map1");
            assert((it1->second == k0) && "Keys don't match");
            m_map1.erase(it1);
            m_map0.erase(it0);
            return true;
        }

        bool erase1(const Key1& k1) {
            auto it1 = m_map1.find(k1);
            if (it1 == m_map1.end()) {
                return false;
            }
            const Key0& k0 = it1->second;
            auto it0 = m_map0.find(k0);
            assert((it0 != m_map0.end()) && "k0 of k1 not in map0");
            assert((std::get<0>(it0->second) == k1) && "Keys don't match");
            m_map0.erase(it0);
            m_map1.erase(it1);
            return true;
        }

        bool erase(const iterator& it) { return erase0(it.m_it0->first); }

      private:
        std::unordered_map<Key0, std::tuple<Key1, Value>> m_map0;
        std::unordered_map<Key1, Key0> m_map1;
    };

} // namespace babelwires
