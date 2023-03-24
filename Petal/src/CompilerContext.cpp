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
//  File Name: CompilerContext.cpp
//  Date File Created: 03/24/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "CompilerContext.h"

#include <utility>

namespace ptl
{
const identifier_info* identifier_lookup::find(const std::string& name) const
{
    if (const auto it{ m_identifiers.find(name) }; it != m_identifiers.end())
        return &it->second;
    return nullptr;
}


const identifier_info* identifier_lookup::insert(std::string name, type_handle type_id, u32 index, bool is_global,
                                                 bool is_constant)
{
    return &m_identifiers.emplace(std::move(name), identifier_info{ type_id, index, is_global, is_constant }).first->second;
}

const identifier_info* global_identifier_lookup::create_identifier(std::string name, type_handle type_id, bool is_constant)
{
    return insert(std::move(name), type_id, size(), true, is_constant);
}

const identifier_info* local_identifier_lookup::find(const std::string& name) const
{
    if (const identifier_info * ret{ identifier_lookup::find(name) })
        return ret;
    return m_parent ? m_parent->find(name) : nullptr;
}

const identifier_info* local_identifier_lookup::create_identifier(std::string name, type_handle type_id, bool is_constant)
{
    return insert(std::move(name), type_id, m_next_identifier_index++, false, is_constant);
}

const identifier_info* function_identifier_lookup::create_param(std::string name, type_handle type_id)
{
    return insert(std::move(name), type_id, m_next_param_index++, false, false);
}

type_handle compiler_context::get_handle(const type_t& t)
{
    return m_types.get_handle(t);
}

const identifier_info* compiler_context::find(const std::string& name) const
{
    if (m_locals)
    {
        if (const identifier_info * ret{ m_locals->find(name) })
            return ret;
    }

    return m_globals.find(name);
}

const identifier_info* compiler_context::create_identifier(std::string name, type_handle type_id, bool is_constant)
{
    if (m_locals)
        return m_locals->create_identifier(std::move(name), type_id, is_constant);
    return m_globals.create_identifier(std::move(name), type_id, is_constant);
}

const identifier_info* compiler_context::create_param(std::string name, type_handle type_id) const
{
    return m_params->create_param(std::move(name), type_id);
}

void compiler_context::enter_scope()
{
    m_locals = create_scope<local_identifier_lookup>(std::move(m_locals));
}

void compiler_context::enter_function()
{
    scope params{ create_scope<function_identifier_lookup>() };
    m_params = params.get();
    m_locals = std::move(params);
}

bool compiler_context::leave_scope()
{
    if (!m_locals)
        return false;

    if (m_params == m_locals.get())
        m_params = nullptr;
    m_locals = m_locals->detach_parent();

    return true;
}

} // namespace ptl
