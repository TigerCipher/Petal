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
//  File Name: PushBackStream.cpp
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "PushBackStream.h"

namespace ptl
{
i32 push_back_stream::operator()()
{
    i32 ret{ -1 };
    if (m_stack.empty())
        ret = m_input();
    else
    {
        ret = m_stack.top();
        m_stack.pop();
    }

    if (ret == '\n')
        ++m_line_number;
    ++m_char_index;

    return ret;
}

void push_back_stream::push_back(i32 c)
{
    m_stack.push(c);

    if (c == '\n')
        --m_line_number;

    --m_char_index;
}

} // namespace ptl
