// Macros to save time
#if !defined(_macro_hh_)
#define _macro_h_

#define strEQ(s1, s2) (!strcmp((s1), (s2)))
#define strNE(s1, s2) ( strcmp((s1), (s2)))

#define SMALLER_ONE(a, b) ((a) < (b) ? (a) : (b))
#define LARGER_ONE(a, b) ((a) > (b) ? (a) : (b))

#define GRI_ABS(x) ((x) < 0.0 ? (-(x)) : (x))

// If 'condition' is false, perform action (typically warning).
#define SUGGEST(condition, action_if_not) {			\
    if (!(condition)) {						\
        action_if_not;						\
    }								\
}
// If 'condition' is untrue, perform action (typically err) and return false
#define Require(condition, action_if_not) {			\
    if (!(condition)) {						\
        action_if_not;						\
        return false;						\
    }								\
}
// If 'condition' is untrue, perform action (typically error) and return void
#define Require2(condition, action_if_not) {			\
    if (!(condition)) {						\
        action_if_not;						\
        return;							\
    }								\
}

#define	ShowStr(x) {						\
    gr_textput ((x));						\
}

// Variable assignment macros, which replaces existing values.
#define PUT_VAR(name, value) {					\
    if (!put_var(name , double(value), true))			\
      fatal_err ("Can't store value of \\", name, "\\");	\
}

// Establish return code
#define RETURN_VALUE(s) {					\
    if (!put_syn("\\.return_value.", (s), true)) OUT_OF_MEMORY;	\
}

// Copy string 'c' into string 'n', first deleting old storage for 'n'
#define COPY_STRING(n, c) {                                    \
    if(strlen((n)) < strlen((c))) {                            \
        delete [] (n);                                         \
        (n) = new char[1 + strlen(c)];                         \
        if (!(n)) OUT_OF_MEMORY;                               \
    }                                                          \
    strcpy((n), (c));                                          \
}

// Pretty-print debug info
#if defined (__GNUC__)
#define DEBUG_MESSAGE(s)     {                                              \
    printf("%*s%s", _function_indent, " ", (s));                            \
}
#define DEBUG_FUNCTION_ENTRY {                                             \
    printf("%*s<%s>  # %s:%d\n",_function_indent, " ", __FUNCTION__,__FILE__,__LINE__); \
    _function_indent += 4;                                                 \
}
#define DEBUG_FUNCTION_EXIT {                                              \
    if (_function_indent > 3) {                                            \
        _function_indent -= 4;                                             \
    }                                                                      \
    printf("%*s</%s> # %s:%d\n", _function_indent, " ", __FUNCTION__,__FILE__,__LINE__); \
}
#else
#define DEBUG_MESSAGE(s)     {                                             \
    printf("%*s%s", _function_indent, " ", (s));                           \
}
#define DEBUG_FUNCTION_ENTRY {                                             \
    printf("%*s<%s>  # %s:%d\n",_function_indent, " ", "unknown-function-name", __FILE__,__LINE__); \
    _function_indent += 4;                                                 \
}
#define DEBUG_FUNCTION_EXIT {                                              \
    if (_function_indent > 3) {                                            \
        _function_indent -= 4;                                             \
    }                                                                      \
    printf("%*s</%s> # %s:%d\n", _function_indent, " ", "unknown-function-name", __FILE__, __LINE__); \
}
#endif


// Optional printing.  BUG: only works for GCC
#if defined(__GNUC__)
#if __GNUC__ >= 3
#define gri_debug_printf(debug_level, format, ...) {\
    if (_debugFlag>=(debug_level)) {\
	printf("DEBUG [%s:%d] ",__FILE__,__LINE__);\
	printf((format),## __VA_ARGS__);\
    }\
}
#else
// Older Gcc versions had a different syntax
#define gri_debug_printf(debug_level, format, args...) {\
    if (_debugFlag>=(debug_level)) {\
        printf("DEBUG [%s:%d] ",__FILE__,__LINE__);\
        printf((format),## args);\
    }\
}
#endif
#endif

#endif
