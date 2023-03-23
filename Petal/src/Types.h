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
//  File Name: Types.h
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once
#include <set>
#include <variant>
#include <vector>

#include "Common.h"


namespace ptl::type
{


enum struct simple_type
{
    nothing,
    number,
    string,
};

struct array_type
{
    i32 inner_type_id{};
};

struct function_type;
using type_t      = std::variant<simple_type, array_type, function_type>;
using type_handle = const type_t*;

struct function_type
{
    struct parameter
    {
        type_handle type_id{};
        bool        by_ref{};
    };

    i32                    return_type_id{};
    std::vector<parameter> parameter_type_id{};
};


class registry
{
public:
    registry();

    [[nodiscard]] type_handle get_handle(const type_t& t);

private:
    struct types_less
    {
        bool operator()(const type_t& t1, const type_t& t2) const;
    };

    std::set<type_t, types_less> m_types{};
};

} // namespace ptl::type
