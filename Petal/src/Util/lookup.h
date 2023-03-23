// ------------------------------------------------------------------------------
//
// Petal
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: lookup.h
// Date File Created: 03/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <algorithm>

namespace ptl::utl
{

template<typename Key, typename Value>
class lookup
{
public:
    using value_t        = std::pair<Key, Value>;
    using container_t    = std::vector<value_t>;
    using iterator       = typename container_t::const_iterator;
    using const_iterator = iterator;

    constexpr lookup(std::initializer_list<value_t> init) : m_container{ init } { std::sort(m_container.begin(), m_container.end()); }

    constexpr lookup(container_t container) : m_container{ std::move(container) }
    {
        std::sort(m_container.begin(), m_container.end());
    }

    [[nodiscard]] constexpr const_iterator begin() const { return m_container.begin(); }

    [[nodiscard]] constexpr const_iterator end() const { return m_container.end(); }

    template<typename K>
    const_iterator find(const K& key) const
    {
        const_iterator it{ std::lower_bound(begin(), end(), key, [](const value_t& p, const K& k) { return p.first < k; }) };

        return it != end() && it->first == key ? it : end();
    }

    [[nodiscard]] constexpr size_t size() const { return m_container.size(); }

private:
    container_t m_container{};
};

} // namespace ptl::utl