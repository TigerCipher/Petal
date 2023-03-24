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

namespace
{
bool is_convertable(type_handle type_from, bool lvalue_from, type_handle type_to, bool lvalue_to)
{
    if (type_to == type_registry::void_handle())
        return true;
    if (type_from == type_to)
        return true;
    if (lvalue_to)
        return lvalue_from && type_from == type_to;

    return type_from == type_registry::number_handle() && type_to == type_registry::string_handle();
}
} // anonymous namespace

node::node(compiler_context& context, node_v value, std::vector<node_ptr> children, u32 line_number, u32 char_index) :
    m_value{ std::move(value) }, m_children{ std::move(children) }, m_line_number{ line_number }, m_char_index{ char_index }
{
    const type_handle void_handle{ type_registry::void_handle() };
    const type_handle number_handle{ type_registry::number_handle() };
    const type_handle string_handle{ type_registry::string_handle() };

    std::visit(overloaded{ [&]([[maybe_unused]] const std::string& val) {
                              m_type_id = string_handle;
                              m_lvalue  = false;
                          },
                           [&]([[maybe_unused]] const f64 val) {
                               m_type_id = number_handle;
                               m_lvalue  = false;
                           },
                           [&](const identifier& val) {
                               if (const identifier_info * info{ context.find(val.name) })
                               {
                                   m_type_id = info->type_id;
                                   m_lvalue  = !info->is_constant;
                               } else
                               {
                                   // TODO: Undeclared error
                               }
                           },
                           [&](const node_operation val) {
                               switch (val)
                               {
                               case node_operation::param:
                                   m_type_id = m_children[0]->type_id();
                                   m_lvalue  = false;
                                   break;
                               case node_operation::preinc:
                               case node_operation::predec:
                                   m_type_id = number_handle;
                                   m_lvalue  = true;
                                   m_children[0]->check_conversion(number_handle, true);
                                   break;
                               case node_operation::postinc:
                               case node_operation::postdec:
                                   m_type_id = number_handle;
                                   m_lvalue  = false;
                                   m_children[0]->check_conversion(number_handle, true);
                                   break;
                               case node_operation::positive:
                               case node_operation::negative:
                               case node_operation::bnot:
                               case node_operation::lnot:
                                   m_type_id = number_handle;
                                   m_lvalue  = false;
                                   m_children[0]->check_conversion(number_handle, false);
                                   break;
                               case node_operation::add:
                               case node_operation::sub:
                               case node_operation::mul:
                               case node_operation::div:
                               case node_operation::idiv:
                               case node_operation::mod:
                               case node_operation::band:
                               case node_operation::bor:
                               case node_operation::bxor:
                               case node_operation::bsl:
                               case node_operation::bsr:
                               case node_operation::land:
                               case node_operation::lor:
                                   m_type_id = number_handle;
                                   m_lvalue  = false;
                                   m_children[0]->check_conversion(number_handle, false);
                                   m_children[1]->check_conversion(number_handle, false);
                                   break;
                               case node_operation::concat:
                                   m_type_id = context.get_handle(simple_type::string);
                                   m_lvalue  = false;
                                   m_children[0]->check_conversion(string_handle, false);
                                   m_children[1]->check_conversion(string_handle, false);
                                   break;
                               case node_operation::assign:
                                   m_type_id = m_children[0]->type_id();
                                   m_lvalue  = true;
                                   m_children[0]->check_conversion(m_type_id, true);
                                   m_children[1]->check_conversion(m_type_id, false);
                                   break;
                               case node_operation::add_assign:
                               case node_operation::sub_assign:
                               case node_operation::mul_assign:
                               case node_operation::div_assign:
                               case node_operation::idiv_assign:
                               case node_operation::mod_assign:
                               case node_operation::band_assign:
                               case node_operation::bor_assign:
                               case node_operation::bxor_assign:
                               case node_operation::bsl_assign:
                               case node_operation::bsr_assign:
                                   m_type_id = number_handle;
                                   m_lvalue  = true;
                                   m_children[0]->check_conversion(number_handle, true);
                                   m_children[1]->check_conversion(number_handle, false);
                                   break;
                               case node_operation::concat_assign:
                                   m_type_id = string_handle;
                                   m_lvalue  = true;
                                   m_children[0]->check_conversion(string_handle, true);
                                   m_children[1]->check_conversion(string_handle, false);
                                   break;
                               case node_operation::eq:
                               case node_operation::ne:
                               case node_operation::lt:
                               case node_operation::gt:
                               case node_operation::le:
                               case node_operation::ge:
                                   m_type_id = number_handle;
                                   m_lvalue  = false;
                                   if (!m_children[0]->is_number() || !m_children[1]->is_number())
                                   {
                                       m_children[0]->check_conversion(string_handle, false);
                                       m_children[1]->check_conversion(string_handle, false);
                                   } else
                                   {
                                       m_children[0]->check_conversion(number_handle, false);
                                       m_children[1]->check_conversion(number_handle, false);
                                   }
                                   break;
                               case node_operation::comma:
                                   for (i32 i{ 0 }; i < (i32) m_children.size(); ++i)
                                   {
                                       m_children[i]->check_conversion(void_handle, false);
                                   }
                                   m_type_id = m_children.back()->type_id();
                                   m_lvalue  = m_children.back()->lvalue();
                                   break;
                               case node_operation::index:
                                   if (const array_type * at{ std::get_if<array_type>(m_children[0]->type_id()) })
                                   {
                                       m_type_id = at->inner_type_id;
                                       m_lvalue  = m_children[0]->lvalue();
                                   } else
                                   {
                                       // TODO: Semantic error
                                   }
                                   break;
                               case node_operation::ternary:
                                   m_children[0]->check_conversion(number_handle, false);
                                   if (is_convertable(m_children[2]->type_id(), m_children[2]->lvalue(), m_children[1]->type_id(),
                                                      m_children[1]->lvalue()))
                                   {
                                       m_children[2]->check_conversion(m_children[1]->type_id(), m_children[1]->lvalue());
                                       m_type_id = m_children[1]->type_id();
                                       m_lvalue  = m_children[1]->lvalue();
                                   } else
                                   {
                                       m_children[1]->check_conversion(m_children[2]->type_id(), m_children[2]->lvalue());
                                       m_type_id = m_children[2]->type_id();
                                       m_lvalue  = m_children[2]->lvalue();
                                   }
                                   break;
                               case node_operation::call:
                                   if (const function_type * ft{ std::get_if<function_type>(m_children[0]->type_id()) })
                                   {
                                       m_type_id = ft->return_type_id;
                                       m_lvalue  = false;
                                       if (ft->parameter_type_id.size() + 1 != m_children.size())
                                       {
                                           // TODO: Semantic error, incorrect argument count
                                       }
                                       for (u32 i{ 0 }; i < ft->parameter_type_id.size(); ++i)
                                       {
                                           if (m_children[i + 1]->lvalue() && !ft->parameter_type_id[i].by_ref)
                                           {
                                               // TODO: Semantic error
                                           }

                                           m_children[i + 1]->check_conversion(ft->parameter_type_id[i].type_id,
                                                                               ft->parameter_type_id[i].by_ref);
                                       }
                                   } else
                                   {
                                       // TODO: Semantic error, function not callable
                                   }
                                   break;
                               }
                           } },
               m_value);
}

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

std::string_view node::get_identifier() const
{
    return std::get<identifier>(m_value).name;
}

f64 node::get_number() const
{
    return std::get<f64>(m_value);
}

std::string_view node::get_string() const
{
    return std::get<std::string>(m_value);
}

void node::check_conversion(type_handle type_id, bool lvalue) const
{
    if(!is_convertable(m_type_id, m_lvalue, type_id, lvalue))
    {
        // TODO: Incorrect type error
    }
}

} // namespace ptl
