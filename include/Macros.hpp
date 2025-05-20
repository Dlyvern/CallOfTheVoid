#ifndef MACROS_HPP
#define MACROS_HPP

#include <memory>
#include "ScriptsRegister.hpp"

#define REGISTER_SCRIPT(SCRIPT_CLASS) \
namespace { \
struct SCRIPT_CLASS##Registrator { \
SCRIPT_CLASS##Registrator() { \
ScriptsRegister::instance().registerScript(#SCRIPT_CLASS, []() { \
return std::make_shared<SCRIPT_CLASS>(); \
}); \
} \
}; \
static SCRIPT_CLASS##Registrator global_##SCRIPT_CLASS##Registrator; \
}


#endif //MACROS_HPP
