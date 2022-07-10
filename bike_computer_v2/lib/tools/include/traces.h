#ifndef TRACES_H
#define TRACES_H


// #if 1
// 	#define DEBUG_OLED(__info,...) printf("[DEBUG_OLED] : " __info,##__VA_ARGS__)
// #else
// 	#define DEBUG_OLED(__info,...)  
// #endif

#define TRACES_ON(id, name) \
    do{\
        tracesOn[name] |= (1 << id);\
    }while (0)

enum tracesE{
    mainE,
    BUTTONS,
    NO_TRACES
};


int tracesOn[NO_TRACES] = {0};

static inline void tracesSetup()
{
    //TRACES_ON(1, mainE); 
    TRACES_ON(0, BUTTONS);
    TRACES_ON(1, BUTTONS);

}

namespace utility {

    template <typename T, size_t S>
    inline constexpr size_t get_file_name_offset(const T (& str)[S], size_t i = S - 1)
    {
        return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
    }

    template <typename T>
    inline constexpr size_t get_file_name_offset(T (& str)[1])
    {
        return 0;
    }

    template <typename T, T v>
    struct const_expr_value
    {
        static constexpr const T value = v;
    };

}

#define UTILITY_CONST_EXPR_VALUE(exp) ::utility::const_expr_value<decltype(exp), exp>::value


#define TRACE_DEBUG(id, name, __info,...) \
    do{\
    if (tracesOn[name] & (1 << id))\
    {\
        printf("[" #name ".%2u]<%s:%d> " __info,\
            id,\
            &__FILE__[UTILITY_CONST_EXPR_VALUE(utility::get_file_name_offset(__FILE__))],\
            __LINE__,\
            ##__VA_ARGS__);\
    }\
    }while(0)

#endif