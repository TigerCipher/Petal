//  ------------------------------------------------------------------------------
//
//  Petal
//     Copyright 2023 Matthew Rogers
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
//     Unless required by applicable law or agreed to in writing, software
//     distributed under the License is distributed on an "AS IS" BASIS,
//     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//     See the License for the specific language governing permissions and
//     limitations under the License.
//
//  File Name: CompilerContext.h
//  Date File Created: 03/23/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Types.h"

#include <unordered_map>
#include <string>

namespace ptl
{
using namespace type;

class identifier_info
{
public:
    identifier_info(type_handle type_id, u32 index, bool is_global, bool is_constant) :
        m_type_id{ type_id }, m_index{ index }, m_is_global{ is_global }, m_is_constant{ is_constant }
    {}

    [[nodiscard]] constexpr type_handle type_id() const { return m_type_id; }
    [[nodiscard]] constexpr u32         index() const { return m_index; }
    [[nodiscard]] constexpr bool        is_global() const { return m_is_global; }
    [[nodiscard]] constexpr bool        is_constant() const { return m_is_constant; }

private:
    type_handle m_type_id{};
    u32         m_index{};
    bool        m_is_global{};
    bool        m_is_constant{};
};

class identifier_lookup
{
public:
    virtual ~identifier_lookup() = default;
    [[nodiscard]] virtual const identifier_info* find(const std::string& name) const;

    virtual void create_identifier(std::string name, type_handle type_id, bool is_constant) = 0;

private:
    std::unordered_map<std::string, identifier_info> m_identifiers{};
};

class global_identifier_lookup : public identifier_lookup
{
public:
    void create_identifier(std::string name, type_handle type_id, bool is_constant) override;
};

class local_identifier_lookup : public identifier_lookup
{
public:
    local_identifier_lookup(scope<local_identifier_lookup> parent) :
        m_parent{ std::move(parent) }, m_next_identifier_index{ m_parent ? m_parent->m_next_identifier_index : 1 }
    {}

    [[nodiscard]] const identifier_info* find(const std::string& name) const override;

    void create_identifier(std::string name, type_handle type_id, bool is_constant) override;

    [[nodiscard]] scope<local_identifier_lookup> detach_parent() { return std::move(m_parent); }

private:
    scope<local_identifier_lookup> m_parent{};
    i32                            m_next_identifier_index{};
};

} // namespace ptl