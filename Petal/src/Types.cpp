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
//  File Name: Types.cpp
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Types.h"

namespace ptl::type
{
registry::registry()
{
    register_type(simple_type::nothing);
    register_type(simple_type::number);
    register_type(simple_type::string);
}

i32 registry::register_type(const type_t& t)
{
    const auto [fst, snd]{ m_types_map.emplace(t, m_types_map.size()) };

    if (snd)
        m_types.push_back(t);

    return fst->second;
}

const type_t& registry::get_type(i32 type_id) const
{
    return m_types[type_id];
}

bool registry::types_less::operator()(const type_t& t1, const type_t& t2) const
{
    const size_t idx1{ t1.index() };

    if (const size_t idx2{ t2.index() }; idx1 != idx2)
        return idx1 < idx2;

    switch (idx1)
    {
    case 0: return std::get<0>(t1) < std::get<0>(t2);
    case 1: return std::get<1>(t1).inner_type_id < std::get<1>(t2).inner_type_id;
    case 2:
    {
        const auto& [return_type_id1, parameter_type_id1]{ std::get<2>(t1) };
        const auto& [return_type_id2, parameter_type_id2]{ std::get<2>(t2) };

        if (return_type_id1 != return_type_id2)
            return return_type_id1 < return_type_id2;
        if (parameter_type_id1.size() != parameter_type_id2.size())
            return parameter_type_id1.size() < parameter_type_id2.size();

        for (u32 i{ 0 }; i < parameter_type_id1.size(); ++i)
        {
            if (parameter_type_id1[i].type_id != parameter_type_id2[i].type_id)
                return parameter_type_id1[i].type_id < parameter_type_id2[i].type_id;
            if (parameter_type_id1[i].by_ref != parameter_type_id2[i].by_ref)
                return parameter_type_id1[i].by_ref < parameter_type_id2[i].by_ref;
        }
    }
    break;
    default: break;
    }

    return false;
}

} // namespace ptl::type
