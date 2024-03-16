#pragma once

#include "core/viper.h"
#include "defines.h"
#include "token.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <optional>

namespace viper {

enum DIRECTIVE {
    INCLUDE,
    DEFINE,
    INVALID
};

/// TODO: Macros
/// -- undecided on language spec for metaprogramming
struct macro {
    std::string name; // identifier for the macro
    
    data_type_primitive type;

    bool is_proc;     // whether it is a proc macro or declaritive
    std::vector<std::string> params;
    
};

class Preprocessor {
    public:
        ~Preprocessor() {}
        Preprocessor create_new(const std::list<token>& input_tokens);
        Preprocessor create_new(VFile* parent, const std::list<token>& input_tokens);

        std::list<token> process(); // Preprocess tokens and 

    private:
        Preprocessor() {}

        void _next_token();
        std::optional<VError> _handle_import();

        VFile* m_parent_file;
        std::unordered_map<std::string, macro> m_macros;
        std::list<token> m_tokens;
        token m_current_token;
        token m_peek_token;
        std::list<token>::iterator m_current_position;
};

}