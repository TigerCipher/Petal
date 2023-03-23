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
//  File Name: Main.cpp
//  Date File Created: 03/21/2023
//  Author: Matt
// 
//  ------------------------------------------------------------------------------

#include "Common.h"
#include <iostream>
#include <sstream>

#include "Tokens.h"
#include "Debug/Errors.h"

int main(int argc, char* argv[])
{
    using namespace ptl;

    std::cerr << "Petal> " << std::endl;
    std::string line{};

    do
    {
        if(std::cin.eof()) std::cin.clear();

        std::getline(std::cin, line);
        if(!line.empty())
        {
            std::istringstream iss{line};
            get_character input { [&iss] {
                return iss.get();
            } };

            try
            {
                push_back_stream stream{input};
                for(token tk { tokenize(stream) }; !tk.is_eof(); tk = tokenize(stream))
                {
                    if(tk.is_reserved_token())
                        std::cout << "Reserved: " << tk.reserved_token() << std::endl;
                    else if(tk.is_identifier())
                        std::cout << "Identifier: " << tk.identifier() << std::endl;
                    else if(tk.is_number())
                        std::cout << "Number: " << tk.number() << std::endl;
                    else if(tk.is_string())
                        std::cout << "String: " << tk.string() << std::endl;
                }
            }catch(const error::error& err)
            {
                iss.clear();
                iss.seekg(0);
                error::format(err, input, std::cerr);
            }
        }
    }while(!line.empty());

    return 0;
}
