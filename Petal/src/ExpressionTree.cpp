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
//  File Name: ExpressionTree.cpp
//  Date File Created: 03/23/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "ExpressionTree.h"

namespace ptl
{
bool node::is_node_operation() const
{
    return std::holds_alternative<node_operation>(m_value);
}

bool node::is_identifier() const
{
    return std::holds_alternative<identifier>(m_value);
}

bool node::is_number() const
{
    return std::holds_alternative<f64>(m_value);
}

bool node::is_string() const
{
    return std::holds_alternative<std::string>(m_value);
}

node_operation node::get_node_operation() const
{
    return std::get<node_operation>(m_value);
}

f64 node::get_number() const
{
    return std::get<f64>(m_value);
}

std::string_view node::get_string() const
{
    return std::get<std::string>(m_value);
}

} // namespace ptl
