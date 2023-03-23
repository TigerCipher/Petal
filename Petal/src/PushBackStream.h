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
//  File Name: PushBackStream.h
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once
#include "Common.h"

#include <stack>

namespace ptl
{
class push_back_stream
{
public:
    push_back_stream(const get_character& input) : m_input{ input } {}

    i32 operator()();

    void push_back(i32 c);

    u32 line_number() const { return m_line_number; }
    u32 char_index() const { return m_char_index; }
private:
    const get_character& m_input{};
    std::stack<i32>      m_stack{};
    u32                  m_line_number{};
    u32                  m_char_index{};
};
} // namespace ptl