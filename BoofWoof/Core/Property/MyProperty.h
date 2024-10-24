#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <array>

namespace xproperty::settings
{
    // Details about the worst-case memory alignment for properties
    namespace details
    {
        constexpr auto force_minimum_case_alignment = 16;

        template<typename... T>
        struct alignas(force_minimum_case_alignment) atomic_type_worse_alignment_and_memory
        {
            alignas(std::max({ alignof(T)... })) std::uint8_t m_Data[std::max({ sizeof(T)... })];
        };
    }

    using data_memory = details::atomic_type_worse_alignment_and_memory<
        std::string,
        std::size_t,
        std::uint64_t
    >;

    using iterator_memory = details::atomic_type_worse_alignment_and_memory<
        std::vector<data_memory>::iterator,
        std::map<std::string, data_memory>::iterator,
        std::uint64_t,
        std::array<char, 4>
    >;
}

#include "../xproperty-master/source/xproperty.h"

namespace xproperty::settings
{
    struct context
    {
        // No context is needed for this example...
    };
}


namespace xproperty::settings
{
    // Note the pattern here. We use the **var_defaults** template to define that
    // we are dealing with your typical atomic type. We also define the name is
    // going to be used to identify it. This name is used to generate the GUID
    // for this type. The GUID is used to identify the type at runtime.
    template<>
    struct var_type<std::int64_t> : var_defaults<"s64", std::int64_t >
    {
        // We put this here for convenience. Since later own we can use this to
        // identify our type base on a single character. Which is useful for
        // serialization and such. But this is not needed for xproperty to work.
        constexpr static inline char ctype_v = 'D';
    };

    template<>
    struct var_type<std::uint64_t> : var_defaults<"u64", std::uint64_t >
    {
        constexpr static inline char ctype_v = 'U';
    };

    template<>
    struct var_type<std::int32_t> : var_defaults<"s32", std::int32_t >
    {
        constexpr static inline char ctype_v = 'd';
    };

    template<>
    struct var_type<std::uint32_t> : var_defaults<"u32", std::uint32_t >
    {
        constexpr static inline char ctype_v = 'u';
    };

    template<>
    struct var_type<std::int16_t> : var_defaults<"s16", std::int16_t >
    {
        constexpr static inline char ctype_v = 'C';
    };

    template<>
    struct var_type<std::uint16_t> : var_defaults<"u16", std::uint16_t >
    {
        constexpr static inline char ctype_v = 'V';
    };

    template<>
    struct var_type<std::int8_t> : var_defaults<"s8", std::int8_t >
    {
        constexpr static inline char ctype_v = 'c';
    };

    template<>
    struct var_type<std::uint8_t> : var_defaults<"u8", std::uint8_t >
    {
        constexpr static inline char ctype_v = 'v';
    };

    template<>
    struct var_type<char> : var_type<std::int8_t> {};

    template<>
    struct var_type<bool> : var_defaults<"bool", bool >
    {
        constexpr static inline char ctype_v = 'b';
    };

    template<>
    struct var_type<float> : var_defaults<"f32", float >
    {
        constexpr static inline char ctype_v = 'f';
    };

    template<>
    struct var_type<double> : var_defaults<"f64", double >
    {
        constexpr static inline char ctype_v = 'F';
    };

#if __has_include(<string>)
    template<>
    struct var_type<std::string> : var_defaults<"string", std::string >
    {
        constexpr static inline char ctype_v = 's';
    };
#endif
}

namespace xproperty::settings
{
    template<typename T>
        requires(std::is_function_v<T> == false)
    struct var_type<T*> : var_ref_defaults<"pointer", T*> {};

    template<typename T>
        requires(std::is_function_v<T> == false)
    struct var_type<T**> : var_ref_defaults<"ppointer", T**> {};
}

namespace xproperty::settings
{
#if __has_include(<memory>)
    template< typename T>
        requires(std::is_function_v<T> == false)
    struct var_type<std::unique_ptr<T>> : var_ref_defaults<"unique_ptr", std::unique_ptr<T>> {};

    template< typename T>
        requires(std::is_function_v<T> == false)
    struct var_type<std::shared_ptr<T>> : var_ref_defaults<"shared_ptr", std::shared_ptr<T>> {};
#endif
}

namespace xproperty::settings
{
#if __has_include(<span>)
    template< typename T >
    struct var_type<std::span<T>> : var_list_defaults< "span", std::span<T>, T > {};
#endif

#if __has_include(<array>)
    template< typename T, auto N >
    struct var_type<std::array<T, N>> : var_list_defaults< "array", std::array<T, N>, T> {};
#endif

#if __has_include(<vector>)
    namespace details
    {
        template <typename...> struct is_std_unique_ptr final : std::false_type {};
        template<class T, typename... Args>
        struct is_std_unique_ptr<std::unique_ptr<T, Args...>> final : std::true_type {};
    }
    template< typename T >
    struct var_type<std::vector<T>> : var_list_defaults< "vector", std::vector<T>, T>
    {
        using type = typename var_list_defaults< "vector", std::vector<T>, T>::type;
        constexpr static void setSize(type& MemberVar, const std::size_t Size, context&) noexcept
        {
            MemberVar.resize(Size);

            // If it is a vector of a unique pointers we can initialize them right away...
            // Otherwise, when we try to access the nullptr pointer, and it will blow up... 
            if constexpr (details::is_std_unique_ptr<T>::value)
            {
                for (auto& E : MemberVar)
                    E = std::make_unique<typename T::element_type>();
            }
        }
    };
#endif
}

namespace xproperty::settings
{
    template< typename T, std::size_t N >
    struct var_type<T[N]> : var_list_native_defaults< "C-Array1D", N, T[N], T, std::size_t > {};

    template< typename T, std::size_t N1, std::size_t N2 >
    struct var_type<T[N1][N2]> : var_list_native_defaults< "C-Array2D", N1, T[N1][N2], T[N2], std::size_t > {};

    template< typename T, std::size_t N1, std::size_t N2, std::size_t N3>
    struct var_type<T[N1][N2][N3]> : var_list_native_defaults< "C-Array3D", N1, T[N1][N2][N3], T[N2][N3], std::size_t > {};
}


namespace xproperty::settings
{
    template< typename T_KEY, typename T_VALUE_TYPE >
    struct var_type<std::map<T_KEY, T_VALUE_TYPE>> : var_list_defaults< "map", std::map<T_KEY, T_VALUE_TYPE>, T_VALUE_TYPE, typename std::map<T_KEY, T_VALUE_TYPE>::iterator, T_KEY >
    {
        using base = var_list_defaults< "map", std::map<T_KEY, T_VALUE_TYPE>, T_VALUE_TYPE, typename std::map<T_KEY, T_VALUE_TYPE>::iterator, T_KEY >;
        using type = typename base::type;
        using specializing_t = typename base::specializing_t;
        using begin_iterator = typename base::begin_iterator;
        using atomic_key = typename base::atomic_key;

        constexpr static void             IteratorToKey(const type& MemberVar, any& Key, const begin_iterator& I, context&) noexcept { Key.set<atomic_key>(I->first); }
        constexpr static specializing_t* getObject(type& MemberVar, const any& Key, context&) noexcept
        {
            auto p = MemberVar.find(Key.get<atomic_key>());

            // if we don't find it then we are going to add it!
            if (p == MemberVar.end())
            {
                MemberVar[Key.get<atomic_key>()] = T_VALUE_TYPE{};
                p = MemberVar.find(Key.get<atomic_key>());
            }

            return &p->second;
        }
        constexpr static specializing_t* IteratorToObject(type& MemberVar, begin_iterator& I, context&) noexcept { return &(*I).second; }
    };
}


namespace xproperty::settings
{
    template< typename T>
    struct llist
    {
        struct node
        {
            std::unique_ptr<node>  m_pNext;
            T                      m_Data;
        };

        // You are required to implement a simple iterator
        // Which includes the following functions
        struct iterator
        {
            node* m_pCurrent;
            std::size_t m_Index;

            iterator    operator ++ ()                        noexcept { m_pCurrent = m_pCurrent->m_pNext.get(); ++m_Index; return *this; }
            bool        operator != (const iterator& I) const noexcept { return m_pCurrent != I.m_pCurrent; }
            T& operator *()                          noexcept { return m_pCurrent->m_Data; }
        };

        // Your container should support the following functions
        // begin, end, and size are require...
        std::size_t     size()                  const noexcept { return m_Count; }
        iterator        begin()                  const noexcept { return { m_pHead.get(), 0 }; }
        iterator        end()                  const noexcept { return { nullptr, m_Count }; }

        // The following functions are not required but are useful
        T* find(const T& Key)      const noexcept
        {
            for (auto& E : *this)
                if (E == Key)
                    return &E;
            return nullptr;
        }

        llist() = default;
        llist(llist&& List) noexcept
            : m_pHead{ std::move(List.m_pHead) }
            , m_Count{ List.m_Count }
        {
            List.m_Count = 0;
        }

        void push_front(T&& Data) noexcept
        {
            auto pNewNode = std::make_unique<node>();
            pNewNode->m_Data = std::move(Data);
            pNewNode->m_pNext = std::move(m_pHead);
            m_pHead = std::move(pNewNode);
            ++m_Count;
        }

        std::unique_ptr<node>   m_pHead = {};
        std::size_t             m_Count = 0;
    };

    //
    // We add the registration/definition of our stupid container here...
    //
    template< typename T >
    struct var_type<llist<T>> : var_list_defaults< "llist", llist<T>, T, typename llist<T>::iterator, T >
    {
        using base = var_list_defaults< "llist", llist<T>, T, typename llist<T>::iterator, T >;
        using type = typename base::type;
        using atomic_key = typename base::atomic_key;
        using specializing_t = typename base::specializing_t;
        using begin_iterator = typename base::begin_iterator;

        constexpr static void IteratorToKey(const type& MemberVar, xproperty::any& Key, const begin_iterator& I, context&) noexcept
        {
            Key.set<atomic_key>(I.m_pCurrent->m_Data);
        }

        constexpr static specializing_t* getObject(type& MemberVar, const any& Key, context&) noexcept
        {
            auto p = MemberVar.find(Key.get<atomic_key>());

            // If we can't find it then we are going to add it!
            if (p == nullptr)
            {
                MemberVar.push_front(T{ Key.get<atomic_key>() });
                p = MemberVar.find(Key.get<atomic_key>());
            }

            return p;
        }
    };
}

namespace xproperty::settings
{
    struct member_help_t : xproperty::member_user_data<"help">
    {
        const char* m_pHelp;
    };
}

//
// We put what the user interface in a different namespace
// to make it less verbose for the user.
//
namespace xproperty
{
    template< xproperty::details::fixed_string HELP >
    struct member_help : settings::member_help_t
    {
        constexpr  member_help() noexcept
            : member_help_t{ .m_pHelp = HELP.m_Value } {}
    };
}

namespace xproperty::meta::details {

    template<std::size_t N>
    struct fixed_string {
        char value[N + 1] = {};

        constexpr fixed_string(const char(&str)[N + 1]) {
            for (std::size_t i = 0; i != N; ++i) {
                value[i] = str[i];
            }
        }

        constexpr operator const char* () const { return value; }
    };
}
