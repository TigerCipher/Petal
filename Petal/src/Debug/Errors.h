// ------------------------------------------------------------------------------
//
// Petal
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: Errors.h
// Date File Created: 03/21/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Common.h"

#include <exception>
#include <string>
#include <ostream>

namespace ptl::error
{

class error final : public std::exception
{
public:
    error(std::string msg, u32 line_number, u32 char_index) noexcept :
        m_message{ std::move(msg) }, m_line_number{ line_number }, m_char_index{ char_index }
    {}

    [[nodiscard]] const char* what() const noexcept override { return m_message.c_str(); }

    [[nodiscard]] u32 line_number() const noexcept { return m_line_number; }
    [[nodiscard]] u32 char_index() const noexcept { return m_char_index; }

private:
    std::string m_message{};
    u32         m_line_number{};
    u32         m_char_index{};
};


error parsing(const char* msg, u32 line_number, u32 char_index);
error unexpected(const std::string_view& unexpected, u32 line_number, u32 char_index);

void format(const error& err, get_character src, std::ostream& output);


} // namespace ptl
