#include "file.h"

#include <cstdlib>
#include <fstream>

namespace viper {

/// @brief Create a file with content read from specified file path
/// @param file_path The path to the file we want to read source code from
VFile VFile::from(const std::string& file_path) {
    VFile file;
    std::size_t file_length;
    std::fstream handle;
    file.name = file_path;
   
    // Open file
    handle.open(file_path.c_str(), std::ios::in);
    if (!handle) {
        std::fprintf(stderr, "Error: cannot open file %s\n", file_path.c_str());
        return {};
    }

    // Find size of file
    handle.seekg(0, handle.end);
    file_length = handle.tellg();
    handle.seekg(0, handle.beg);

    // Read into the content of file
    file.content.resize(file_length);
    handle.read(file.content.data(), file_length);

    return file;
}

} // core namespace
