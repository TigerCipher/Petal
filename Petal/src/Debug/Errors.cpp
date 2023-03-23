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
//  File Name: Errors.cpp
//  Date File Created: 03/21/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Errors.h"

namespace ptl::error
{

error parsing(const char* msg, u32 line_number, u32 char_index)
{
    std::string err_msg{ "Parsing Error: " };
    err_msg += msg;
    return { std::move(err_msg), line_number, char_index };
}

error unexpected(const std::string_view& unexpected, u32 line_number, u32 char_index)
{
    std::string err_msg{ "Unexpected '" };
    err_msg += unexpected;
    err_msg += "'";
    return parsing(err_msg.c_str(), line_number, char_index);
}

void format(const error& err, get_character src, std::ostream& output)
{
    output << "(" << err.line_number() + 1 << ") " << err.what() << std::endl;

    u32 char_idx{ 0 };

    for (u32 line_number{ 0 }; line_number < err.line_number(); ++char_idx)
    {
        const i32 c{ src() };
        if (c < 0)
            return;
        if (c == '\n')
            ++line_number;
    }

    const u32 idx_in_line{ err.char_index() - char_idx };

    std::string line{};
    for (u32 i{ 0 };; ++i)
    {
        const i32 c{ src() };
        if (c < 0 || c == '\n' || c == '\r')
            break;
        line += (char) c;
    }

    output << line << std::endl;

    for (u32 i{ 0 }; i < idx_in_line; ++i)
    {
        output << " ";
    }

    output << "^" << std::endl;
}


} // namespace ptl::error
