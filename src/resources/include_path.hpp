#ifndef RESOURCES_INCLUDE_PATH_HPP
#define RESOURCES_INCLUDE_PATH_HPP

#include <string>
#include <initializer_list>
#include <vector>

namespace ts
{
    namespace resources
    {
        struct Include_error
            : public std::exception
        {
            Include_error(const std::string& file_name) {};
        };

        std::string find_include_path(const std::string& file_name, std::initializer_list<std::string> search_path);
    }
}

#endif

