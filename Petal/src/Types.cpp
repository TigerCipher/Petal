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

namespace
{
type_t void_type;
type_t number_type;
type_t string_type;
} // anonymous namespace


registry::registry()
{
    m_types.emplace(simple_type::nothing);
    m_types.emplace(simple_type::number);
    m_types.emplace(simple_type::string);
}


type_handle registry::get_handle(const type_t& t)
{
    return std::visit(overloaded{
                          [](const simple_type st) {
                              switch (st)
                              {
                              case simple_type::nothing: return void_handle();
                              case simple_type::number: return number_handle();
                              case simple_type::string: return string_handle();
                              }
                          },
                          [this](const auto& tt) { return &*m_types.insert(tt).first; },
                      },
                      t);
}

type_handle registry::void_handle()
{
    return &void_type;
}

type_handle registry::number_handle()
{
    return &number_type;
}

type_handle registry::string_handle()
{
    return &string_type;
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

namespace std
{
using namespace ptl;
using namespace type;

std::string to_string(type_handle t)
{
    return std::visit(overloaded{
                          [](const simple_type st) {
                              switch (st)
                              {
                              case simple_type::nothing: return std::string{ "void" };
                              case simple_type::number: return std::string{ "number" };
                              case simple_type::string: return std::string{ "string" };
                              }
                          },
                          [](const array_type& at) {
                              std::string ret{ to_string(at.inner_type_id) };
                              ret += "[]";
                              return ret;
                          },
                          [](const function_type& ft) {
                              std::string ret{ to_string(ft.return_type_id) + "(" };
                              auto        separator{ "" };
                              for (const auto& [type_id, by_ref] : ft.parameter_type_id)
                              {
                                  ret += separator + to_string(type_id) + (by_ref ? "&" : "");
                                  separator = ",";
                              }
                              ret += ")";
                              return ret;
                          },
                      },
                      *t);
}

} // namespace std
