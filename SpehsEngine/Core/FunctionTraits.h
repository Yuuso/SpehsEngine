#pragma once
// Modified from: https://www.boost.org/LICENSE_1_0.txt


namespace se
{
    namespace detail
    {
        template<typename Function>
        struct FunctionTraitsHelper;

        template<typename R>
        struct FunctionTraitsHelper<R(*)(void)>
        {
            static const unsigned arity = 0;
            typedef R ResultType;
        };

        template<typename R, typename T1>
        struct FunctionTraitsHelper<R(*)(T1)>
        {
            static const unsigned arity = 1;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T1 argument_type;
        };

        template<typename R, typename T1, typename T2>
        struct FunctionTraitsHelper<R(*)(T1, T2)>
        {
            static const unsigned arity = 2;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
        };

        template<typename R, typename T1, typename T2, typename T3>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3)>
        {
            static const unsigned arity = 3;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
        };

        template<typename R, typename T1, typename T2, typename T3, typename T4>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3, T4)>
        {
            static const unsigned arity = 4;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
            typedef T4 Arg4Type;
        };

        template<typename R, typename T1, typename T2, typename T3, typename T4,
            typename T5>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3, T4, T5)>
        {
            static const unsigned arity = 5;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
            typedef T4 Arg4Type;
            typedef T5 Arg5Type;
        };

        template<typename R, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3, T4, T5, T6)>
        {
            static const unsigned arity = 6;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
            typedef T4 Arg4Type;
            typedef T5 Arg5Type;
            typedef T6 Arg6Type;
        };

        template<typename R, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename T7>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3, T4, T5, T6, T7)>
        {
            static const unsigned arity = 7;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
            typedef T4 Arg4Type;
            typedef T5 Arg5Type;
            typedef T6 Arg6Type;
            typedef T7 Arg7Type;
        };

        template<typename R, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename T7, typename T8>
        struct FunctionTraitsHelper<R(*)(T1, T2, T3, T4, T5, T6, T7, T8)>
        {
            static const unsigned arity = 8;
            typedef R ResultType;
            typedef T1 Arg1Type;
            typedef T2 Arg2Type;
            typedef T3 Arg3Type;
            typedef T4 Arg4Type;
            typedef T5 Arg5Type;
            typedef T6 Arg6Type;
            typedef T7 Arg7Type;
            typedef T8 Arg8Type;
        };
    }

    template<typename Function>
    struct FunctionTraits : public detail::FunctionTraitsHelper<typename std::add_pointer<Function>::type>
    {
    };
}
