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
//  File Name: Tokens.cpp
//  Date File Created: 03/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Tokens.h"
#include "Util/Lookup.h"
#include "Debug/Errors.h"

namespace ptl
{
namespace
{

constexpr i32 tab{ 't' };
constexpr i32 new_line{ 'n' };
constexpr i32 carriage_return{ 'r' };
constexpr i32 null_term{ '0' };

const utl::lookup<std::string_view, reserved_token> operator_token_map{
    { "++",           reserved_token::inc},
    { "--",           reserved_token::dec},
    {  "+",           reserved_token::add},
    { "..",        reserved_token::concat},
    {  "-",           reserved_token::sub},
    {  "*",           reserved_token::mul},
    {  "/",           reserved_token::div},
    { "\\",          reserved_token::idiv},
    {  "%",           reserved_token::mod},
    {  "~",   reserved_token::bitwise_not},
    {  "&",   reserved_token::bitwise_and},
    {  "|",    reserved_token::bitwise_or},
    {  "^",   reserved_token::bitwise_xor},
    { "<<",        reserved_token::shiftl},
    { ">>",        reserved_token::shiftr},
    {  "=",        reserved_token::assign},
    { "+=",    reserved_token::add_assign},
    {"..=", reserved_token::concat_assign},
    { "-=",    reserved_token::sub_assign},
    { "*=",    reserved_token::mul_assign},
    { "/=",    reserved_token::div_assign},
    {"\\=",   reserved_token::idiv_assign},
    { "%=",    reserved_token::mod_assign},
    { "&=",    reserved_token::and_assign},
    { "|=",     reserved_token::or_assign},
    { "^=",    reserved_token::xor_assign},
    {"<<=", reserved_token::shiftl_assign},
    {">>=", reserved_token::shiftr_assign},
    {  "!",   reserved_token::logical_not},
    { "&&",   reserved_token::logical_and},
    { "||",    reserved_token::logical_or},
    { "==",            reserved_token::eq},
    { "!=",            reserved_token::ne},
    {  "<",            reserved_token::lt},
    {  ">",            reserved_token::gt},
    { "<=",            reserved_token::le},
    { ">=",            reserved_token::ge},
    {  "?",      reserved_token::question},
    {  ":",         reserved_token::colon},
    {  ",",         reserved_token::comma},
    {  ";",     reserved_token::semicolon},
    {  "(",    reserved_token::open_round},
    {  ")",   reserved_token::close_round},
    {  "{",    reserved_token::open_curly},
    {  "}",   reserved_token::close_curly},
    {  "[",   reserved_token::open_square},
    {  "]",  reserved_token::close_square},
};

const utl::lookup<std::string_view, reserved_token> keyword_token_map{
    {      "if",       reserved_token::kw_if},
    {    "else",     reserved_token::kw_else},
    {    "elif",     reserved_token::kw_elif},
    {  "switch",   reserved_token::kw_switch},
    {    "case",     reserved_token::kw_case},
    { "default",  reserved_token::kw_default},
    {     "for",      reserved_token::kw_for},
    {   "while",    reserved_token::kw_while},
    {      "do",       reserved_token::kw_do},
    {   "break",    reserved_token::kw_break},
    {"continue", reserved_token::kw_continue},
    {  "return",   reserved_token::kw_return},
    {     "var",      reserved_token::kw_var},
    {     "fun",      reserved_token::kw_fun},
    {    "void",     reserved_token::kw_void},
    {  "number",   reserved_token::kw_number},
    {  "string",   reserved_token::kw_string},
};

const utl::lookup token_string_map{ [] {
    std::vector<std::pair<reserved_token, std::string_view>> container{};
    container.reserve(operator_token_map.size() + keyword_token_map.size());
    for (const auto& p : operator_token_map)
    {
        container.emplace_back(p.second, p.first);
    }
    for (const auto& p : keyword_token_map)
    {
        container.emplace_back(p.second, p.first);
    }

    return utl::lookup(std::move(container));
}() };

enum struct character_type
{
    eof,
    space,
    alphanum,
    punct,
};

} // anonymous namespace

std::ostream& operator<<(std::ostream& os, reserved_token tk)
{
    os << token_string_map.find(tk)->second;
    return os;
}

std::optional<reserved_token> get_keyword(std::string_view word)
{
    const auto it{ keyword_token_map.find(word) };
    return it == keyword_token_map.end() ? std::nullopt : std::make_optional(it->second);
}

namespace
{
class comparator
{
public:
    constexpr comparator(u32 idx) : m_index{ idx } {}

    bool operator()(char l, char r) const { return l < r; }

    bool operator()(std::pair<std::string_view, reserved_token> l, char r) const
    {
        return l.first.size() <= m_index || l.first[m_index] < r;
    }

    bool operator()(char l, std::pair<std::string_view, reserved_token> r) const
    {
        return r.first.size() > m_index && l < r.first[m_index];
    }

    bool operator()(std::pair<std::string_view, reserved_token> l, std::pair<std::string_view, reserved_token> r) const
    {
        return r.first.size() > m_index && (l.first.size() < m_index || l.first[m_index] < r.first[m_index]);
    }

private:
    u32 m_index{};
};

character_type get_character_type(i32 c)
{
    if (c < 0)
        return character_type::eof;
    if (std::isspace(c))
        return character_type::space;
    if (std::isalpha(c) || std::isdigit(c) || c == '_')
        return character_type::alphanum;

    return character_type::punct;
}

} // anonymous namespace

bool token::is_reserved_token() const
{
    return std::holds_alternative<enum reserved_token>(m_value);
}

bool token::is_identifier() const
{
    return std::holds_alternative<struct identifier>(m_value);
}

bool token::is_number() const
{
    return std::holds_alternative<f64>(m_value);
}

bool token::is_string() const
{
    return std::holds_alternative<std::string>(m_value);
}

bool token::is_eof() const
{
    return std::holds_alternative<eof>(m_value);
}

reserved_token token::reserved_token() const
{
    return std::get<enum reserved_token>(m_value);
}

std::string_view token::identifier() const
{
    return std::get<struct identifier>(m_value).name;
}

f64 token::number() const
{
    return std::get<f64>(m_value);
}

std::string_view token::string() const
{
    return std::get<std::string>(m_value);
}

namespace
{
token fetch_word(push_back_stream& stream)
{
    const u32 line_number{ stream.line_number() };
    const u32 char_index{ stream.char_index() };

    std::string word{};
    i32         c{ stream() };

    const bool is_number = isdigit(c);

    do
    {
        word.push_back((char) c);
        c = stream();
    } while (get_character_type(c) == character_type::alphanum || (is_number && c == '.'));

    stream.push_back(c);

    if (const std::optional tk{ get_keyword(word) })
    {
        return token{ *tk, line_number, char_index };
    }
    if (std::isdigit(word.front()))
    {
        char* endptr;
        f64   num{ (f64) strtol(word.c_str(), &endptr, 0) };
        if (*endptr != 0)
        {
            num = strtod(word.c_str(), &endptr);
            if (*endptr != 0)
            {
                const size_t remaining{ word.size() - (endptr - word.c_str()) };
                throw error::unexpected(std::string{ 1, *endptr }, stream.line_number(), stream.char_index() - (u32) remaining);
            }
        }

        return token{ num, line_number, char_index };
    }

    return token{ identifier{ std::move(word) }, line_number, char_index };
}

token fetch_operator(push_back_stream& stream)
{
    const u32 line_number{ stream.line_number() };
    const u32 char_index{ stream.char_index() };

    if (const std::optional tk{ get_operator(stream) })
        return token{ *tk, line_number, char_index };

    std::string unexpected{};
    const u32   error_line_number{ stream.line_number() };
    const u32   error_char_index{ stream.char_index() };

    for (i32 c = stream(); get_character_type(c) == character_type::punct; c = stream())
    {
        unexpected.push_back((char) c);
    }

    throw error::unexpected(unexpected, error_line_number, error_char_index);
}

token fetch_string(push_back_stream& stream)
{
    const u32 line_number{ stream.line_number() };
    const u32 char_index{ stream.char_index() };

    std::string str{};

    bool escaped{ false };
    i32  c{ stream() };

    for (; get_character_type(c) != character_type::eof; c = stream())
    {
        if (c == '\\')
            escaped = true;
        else if (escaped)
        {
            switch (c)
            {
            case tab: str.push_back('\t'); break;
            case new_line: str.push_back('\n'); break;
            case carriage_return: str.push_back('\r'); break;
            case null_term: str.push_back('\0'); break;
            default: str.push_back(c);
            }
        } else
        {
            switch (c)
            {
            case '\t':
            case '\n':
            case '\r':
                stream.push_back(c);
                throw error::parsing("Expected closing '\"'", stream.line_number(), stream.char_index());
            case '"': return token{ std::move(str), line_number, char_index };
            default: str.push_back(c);
            }
        }
    }

    stream.push_back(c);
    throw error::parsing("Expected closing '\"'", stream.line_number(), stream.char_index());
}

// Skip over a line a single line, convention is that Petal comments are like C/C++ comments. I.e, it begins with //
void skip_line_comment(push_back_stream& stream)
{
    i32 c;
    do
    {
        c = stream();
    } while (c != '\n' && get_character_type(c) != character_type::eof);

    if (c != '\n')
        stream.push_back(c);
}

// Skip over a block of lines, convention is that Petal comments are like C/C++ comments. I.e, it begins with /* and ends with */
void skip_block_comment(push_back_stream& stream)
{
    bool closing = false;
    i32  c;
    do
    {
        c = stream();
        if (closing && c == '/')
            return;
        closing = c == '*';
    } while (get_character_type(c) != character_type::eof);

    // If we reach here, it means the eof was met, but a closing */ was never encountered
    stream.push_back(c);
    throw error::parsing("Expected closing '*/'", stream.line_number(), stream.char_index());
}

} // anonymous namespace


std::optional<reserved_token> get_operator(push_back_stream& stream)
{
    auto                          candidates{ std::make_pair(operator_token_map.begin(), operator_token_map.end()) };
    std::optional<reserved_token> ret;
    u32                           match_size{ 0 };
    std::stack<i32>               chars;

    for (u32 i{ 0 }; candidates.first != candidates.second; ++i)
    {
        chars.push(stream());
        candidates = std::equal_range(candidates.first, candidates.second, (char) chars.top(), comparator{ i });

        if (candidates.first != candidates.second && candidates.first->first.size() == i + 1)
        {
            match_size = i + 1;
            ret        = candidates.first->second;
        }
    }

    while (chars.size() > match_size)
    {
        stream.push_back(chars.top());
        chars.pop();
    }

    return ret;
}


token tokenize(push_back_stream& stream)
{
    while (true)
    {
        u32 line_number{ stream.line_number() };
        u32 char_index{ stream.char_index() };
        switch (const i32 c{ stream() }; get_character_type(c))
        {
        case character_type::eof: return { eof{}, line_number, char_index };
        case character_type::space: continue;
        case character_type::alphanum: stream.push_back(c); return fetch_word(stream);
        case character_type::punct:
            //{
            switch (c)
            {
            case '"': return fetch_string(stream);
            case '/':
            {
                switch (const i32 ch{ stream() })
                {
                case '/': skip_line_comment(stream); continue;
                case '*': skip_block_comment(stream); continue;
                default: stream.push_back(ch);
                }
            }
            default: stream.push_back(c); return fetch_operator(stream);
            }
            //}
            break;
        }
    }
}

} // namespace ptl