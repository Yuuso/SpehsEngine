#pragma once
#define SPEHS_HAS_MEMBER_FUNCTION(functionName, structName) \
template<typename T, typename Sign> \
struct structName \
{ \
    typedef char yes[1]; \
    typedef char no [2]; \
    template <typename U, U> struct type_check; \
    template <typename _1> static yes &chk(type_check<Sign, &_1::functionName > *); \
    template <typename   > static no  &chk(...); \
    static bool const value = sizeof(chk<T>(0)) == sizeof(yes); \
}
