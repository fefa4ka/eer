#pragma once

// C Pre-Processor Magic
// http://jhnet.co.uk/articles/cpp_magic

/**
 * @brief Returns the first argument passed to it
 * 
 * @param a The first argument
 * @param ... The rest of the arguments
 * @return The first argument
 */
#define FIRST_(a, ...)     a

/**
 * @brief Returns the second argument passed to it
 * 
 * @param a The first argument
 * @param b The second argument
 * @param ... The rest of the arguments
 * @return The second argument
 */
#define SECOND_(a, b, ...) b


/**
 * @brief Returns the first argument passed to it
 * 
 * @param ... The rest of the arguments
 * @return The first argument
 */
#define FIRST(...)  FIRST_(__VA_ARGS__, )

/**
 * @brief Returns the second argument passed to it
 * 
 * @param ... The rest of the arguments
 * @return The second argument
 */
#define SECOND(...) SECOND_(__VA_ARGS__, )


#define EMPTY()

/**
 * @brief Evaluates the arguments passed to it, allowing for multiple levels of indirection
 * 
 * This macro uses preprocessor magic to recursively evaluate the arguments passed to it, 
 * allowing for multiple levels of indirection. It is used to expand macros that may contain 
 * other macros as arguments, allowing for more complex macro functionality.
 * 
 * @param ... The arguments to be evaluated
 * @return The evaluated arguments
 */
#define EVAL(...)     EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...)  EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...)  EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...)  EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...)   EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...)   EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...)   EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...)    EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...)    EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...)    EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...)    __VA_ARGS__


/**
 * @brief Defers the expansion of a macro by one step
 * 
 * This macro is used to defer the expansion of a macro by one step. It is useful when 
 * a macro needs to be expanded after another macro has been expanded, but the order of 
 * expansion is not guaranteed. By deferring the expansion of the macro, it can be 
 * guaranteed that it will be expanded after the other macro.
 * 
 * @param m The macro to be deferred
 * @return The deferred macro
 */
#define DEFER1(m) m EMPTY()

/**
 * @brief Defers the expansion of a macro by two steps
 * 
 * This macro is used to defer the expansion of a macro by two steps. It is useful when 
 * a macro needs to be expanded after another macro has been expanded, but the order of 
 * expansion is not guaranteed. By deferring the expansion of the macro, it can be 
 * guaranteed that it will be expanded after the other macro.
 * 
 * @param m The macro to be deferred
 * @return The deferred macro
 */
#define DEFER2(m) m EMPTY EMPTY()()

/**
 * @brief Returns the second argument passed to it or 0 if there is no second argument
 * 
 * This macro is used to extract the second argument passed to it. If there is no second argument, 
 * it returns 0. It is used in conjunction with the PROBE macro to detect the presence of arguments 
 * in a variadic macro.
 * 
 * @param ... The arguments to be evaluated
 * @return The second argument or 0 if there is no second argument
 */
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)

/**
 * @brief Returns a tuple of two values that can be used to detect the presence of arguments in a variadic macro
 * 
 * This macro returns a tuple of two values that can be used to detect the presence of arguments in a variadic macro. 
 * It is used in conjunction with the IS_PROBE macro to detect the presence of arguments in a variadic macro.
 * 
 * @return A tuple of two values that can be used to detect the presence of arguments in a variadic macro
 */
#define PROBE()       ~, 1

/**
 * @brief Concatenates two tokens into a single token
 * 
 * This macro concatenates two tokens into a single token. 
 * 
 * @param a The first token
 * @param b The second token
 * @return The concatenated token
 */
#define CAT(a, b) a##b

#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define IF_ELSE(condition)  _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...) _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define HAS_ARGS(...)        BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

/**
 * @brief Applies a macro to each argument in a variadic list
 * 
 * This macro applies a given macro to each argument in a variadic list. 
 * 
 * @param m The macro to apply to each argument
 * @param first The first argument in the list
 * @param ... The rest of the arguments in the list
 */
#define MAP(m, first, ...)                                                     \
    m(first) IF_ELSE(HAS_ARGS(__VA_ARGS__))(                                   \
        DEFER2(_MAP)()(m, __VA_ARGS__))(/* Do nothing, just terminate */       \
    )
#define _MAP() MAP
