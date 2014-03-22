#include "include_path.hpp"

#include <boost/filesystem.hpp>


std::string ts::resources::find_include_path(const std::string& file_name, std::initializer_list<std::string> search_paths)
{
    for (const auto& path : search_paths)
    {
        boost::filesystem::path full_path = path;
        full_path /= file_name;

        if (is_regular_file(full_path)) return full_path.string();
    }

    throw Include_error(file_name);
}