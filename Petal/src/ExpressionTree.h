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
//  File Name: ExpressionTree.h
//  Date File Created: 03/23/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Tokens.h"
#include "Types.h"

#include <variant>
#include <vector>

namespace ptl
{

enum struct node_operation
{
    preinc,
    predec,
    postinc,
    postdec,
    positive,
    negative,
    bnot,
    lnot,

    add,
    sub,
    mul,
    div,
    idiv,
    mod,
    band,
    bor,
    bxor,
    sl,
    sr,
    concat,
    assign,
    add_assign,
    sub_assign,
    mul_assign,
    div_assign,
    idiv_assign,
    mod_assign,
    band_assign,
    bor_assign,
    bxor_assign,
    bsl_assign,
    bsr_assign,
    concat_assign,
    eq,
    ne,
    lt,
    gt,
    le,
    ge,
    comma,
    land,
    lor,
    index,

    ternary,

    call,
};

struct node;
using node_ptr = std::unique_ptr<node>;

struct node
{
    using node_v = std::variant<node_operation, std::string, f64, identifier>;
    constexpr node(node_v value, std::vector<node_ptr> children, u32 line_number, u32 char_index) :
        m_value{ std::move(value) }, m_children{ std::move(children) }, m_line_number{ line_number }, m_char_index{ char_index }
    {}

    [[nodiscard]] bool is_node_operation() const;
    [[nodiscard]] bool is_identifier() const;
    [[nodiscard]] bool is_number() const;
    [[nodiscard]] bool is_string() const;

    [[nodiscard]] node_operation get_node_operation() const;
    [[nodiscard]] std::string_view get_identifier() const;=
    [[nodiscard]] f64 get_number() const;
    [[nodiscard]] std::string_view get_string() const;

    [[nodiscard]] constexpr const std::vector<node_ptr>& children() const { return m_children; }
    [[nodiscard]] constexpr u32 line_number() const { return m_line_number; }
    [[nodiscard]] constexpr u32 char_index() const { return m_char_index; }

private:
    node_v                m_value{};
    std::vector<node_ptr> m_children{};
    u32                   m_line_number{};
    u32                   m_char_index{};
};

} // namespace ptl