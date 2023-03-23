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
//  File Name: Tokens.h
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once


#include "Common.h"

#include <optional>
#include <ostream>
#include <variant>
#include <string_view>

#include "PushBackStream.h"

namespace ptl
{

enum struct reserved_token
{
    inc,
    dec,
    add,
    sub,
    concat,
    mul,
    div,
    idiv,
    mod,
    bitwise_not,
    bitwise_and,
    bitwise_or,
    bitwise_xor,
    shiftl,
    shiftr,
    assign,
    add_assign,
    sub_assign,
    concat_assign,
    mul_assign,
    div_assign,
    idiv_assign,
    mod_assign,
    and_assign,
    or_assign,
    xor_assign,
    shiftl_assign,
    shiftr_assign,
    logical_not,
    logical_and,
    logical_or,
    eq,
    ne,
    lt,
    gt,
    le,
    ge,
    question,
    colon,
    comma,
    semicolon,
    open_round,
    close_round,
    open_curly,
    close_curly,
    open_square,
    close_square,
    kw_if,
    kw_else,
    kw_elif,
    kw_switch,
    kw_case,
    kw_default,
    kw_for,
    kw_while,
    kw_do,
    kw_break,
    kw_continue,
    kw_return,
    kw_var,
    kw_fun,
    kw_void,
    kw_number,
    kw_string,
};

struct identifier
{
    std::string name{};
};

struct eof
{};

std::ostream& operator<<(std::ostream& os, reserved_token tk);

std::optional<reserved_token> get_keyword(std::string_view word);
std::optional<reserved_token> get_operator(push_back_stream& stream);

class token
{
private:
    using token_v = std::variant<reserved_token, identifier, f64, std::string, eof>;
public:
    token(token_v value, u32 line_number, u32 char_index) : m_value{std::move(value)}, m_line_number{ line_number }, m_char_index{ char_index }{}

    [[nodiscard]] bool is_reserved_token() const;
    [[nodiscard]] bool is_identifier() const;
    [[nodiscard]] bool is_number() const;
    [[nodiscard]] bool is_string() const;
    [[nodiscard]] bool is_eof() const;

    [[nodiscard]] reserved_token reserved_token() const;
    [[nodiscard]] std::string_view identifier() const;
    [[nodiscard]] f64 number() const;
    [[nodiscard]] std::string_view string() const;

    [[nodiscard]] u32 line_number() const { return m_line_number; }
    [[nodiscard]] u32 char_index() const { return m_char_index; }
private:
    token_v m_value{};
    u32     m_line_number{};
    u32     m_char_index{};
};


token tokenize(push_back_stream& stream);

} // namespace ptl
