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

struct identifier_info
{
    type_handle type_id{};
    u32         index{};
    bool        is_global{};
    bool        is_constant{};
};

class identifier_lookup
{
public:
    virtual ~identifier_lookup() = default;
    [[nodiscard]] virtual const identifier_info* find(const std::string& name) const;

    virtual const identifier_info* create_identifier(std::string name, type_handle type_id, bool is_constant) = 0;

protected:
    const identifier_info*      insert(std::string name, type_handle type_id, u32 index, bool is_global, bool is_constant);
    [[nodiscard]] constexpr u32 size() const { return (u32) m_identifiers.size(); }

private:
    std::unordered_map<std::string, identifier_info> m_identifiers{};
};

class global_identifier_lookup final : public identifier_lookup
{
public:
    const identifier_info* create_identifier(std::string name, type_handle type_id, bool is_constant) override;
};

class local_identifier_lookup : public identifier_lookup
{
public:
    local_identifier_lookup(scope<local_identifier_lookup> parent) :
        m_parent{ std::move(parent) }, m_next_identifier_index{ m_parent ? m_parent->m_next_identifier_index : 1 }
    {}

    [[nodiscard]] const identifier_info* find(const std::string& name) const override;

    const identifier_info* create_identifier(std::string name, type_handle type_id, bool is_constant) override;

    [[nodiscard]] scope<local_identifier_lookup> detach_parent() { return std::move(m_parent); }

private:
    scope<local_identifier_lookup> m_parent{};
    i32                            m_next_identifier_index{};
};

class function_identifier_lookup final : public local_identifier_lookup
{
public:
    function_identifier_lookup() : local_identifier_lookup{ nullptr }, m_next_param_index{ -1 } {}
    const identifier_info* create_param(std::string name, type_handle type_id);

private:
    i32 m_next_param_index{};
};

class compiler_context
{
public:
    compiler_context() = default;

    type_handle get_handle(const type_t& t);

    [[nodiscard]] const identifier_info* find(const std::string& name) const;
    [[nodiscard]] const identifier_info* create_identifier(std::string name, type_handle type_id, bool is_constant);
    [[nodiscard]] const identifier_info* create_param(std::string name, type_handle type_id) const;

    void enter_scope();
    void enter_function();
    bool leave_scope();

private:
    function_identifier_lookup*    m_params{ nullptr };
    global_identifier_lookup       m_globals{};
    scope<local_identifier_lookup> m_locals{};
    type_registry                  m_types{};
};

} // namespace ptl