/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BUSTACHE_AST_HPP_INCLUDED
#define BUSTACHE_AST_HPP_INCLUDED

#include <bustache/detail/variant.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/unordered_map.hpp>
#include <vector>
#include <string>

namespace bustache { namespace ast
{
    struct variable;
    struct section;
    class content;

    using text = boost::string_ref;

    using content_list = std::vector<content>;

    using override_map = boost::unordered_map<std::string, content_list>;

    struct null {};

    struct variable
    {
        std::string key;
        char tag = '\0';
    };

    struct block
    {
        std::string key;
        content_list contents;
    };

    struct section : block
    {
        char tag = '#';
    };

    struct partial
    {
        std::string key;
        std::string indent;
        override_map overriders;
    };

#define BUSTACHE_AST_CONTENT(X, D)                                              \
    X(0, null, D)                                                               \
    X(1, text, D)                                                               \
    X(2, variable, D)                                                           \
    X(3, section, D)                                                            \
    X(4, partial, D)                                                            \
    X(5, block, D)                                                              \
/***/

    class content : public variant_base<content>
    {
        struct type_matcher
        {
            BUSTACHE_AST_CONTENT(Zz_BUSTACHE_VARIANT_MATCH, )
        };
        unsigned _which;
        union
        {
            char _storage[1];
            BUSTACHE_AST_CONTENT(Zz_BUSTACHE_VARIANT_MEMBER, )
        };
    public:
        //Zz_BUSTACHE_VARIANT_DECL(content, BUSTACHE_AST_CONTENT, true)
        struct switcher
        {
            template<class T, class Visitor>
            static auto common_ret(T* data, Visitor& v) ->
                decltype(BUSTACHE_AST_CONTENT(Zz_BUSTACHE_VARIANT_RET,) throw bad_variant_access());
            template<class T, class Visitor>
            static auto visit(unsigned which, T* data, Visitor& v) ->
                decltype(common_ret(data, v))
            {
                switch (which)
                {
                BUSTACHE_AST_CONTENT(Zz_BUSTACHE_VARIANT_SWITCH,)
                default: throw bad_variant_access();
                }
            }
            BUSTACHE_AST_CONTENT(Zz_BUSTACHE_VARIANT_INDEX,)
        };
        private:
        void invalidate() noexcept
        {
            if (valid())
            {
                detail::dtor_visitor v;
                switcher::visit(_which, data(), v);
                _which = ~0u;
            }
        }
        template<class T>
        void do_init(T& other)
        {
            detail::ctor_visitor v{_storage};
            switcher::visit(other._which, other.data(), v);
        }
        template<class T>
        void do_assign(T& other)
        {
            if (_which == other._which)
            {
                detail::assign_visitor v{_storage};
                switcher::visit(other._which, other.data(), v);
            }
            else
            {
                invalidate();
                if (other.valid())
                {
                    do_init(other);
                    _which = other._which;
                }
            }
        }
        public:
        unsigned which() const noexcept
        {
            return _which;
        }
        bool valid() const noexcept
        {
            return _which != ~0u;
        }
        void* data() noexcept
        {
            return _storage;
        }
        void const* data() const noexcept
        {
            return _storage;
        }
        content(content&& other) noexcept(true) : _which(other._which)
        {
            do_init(other);
        }
        content(content const& other) : _which(other._which)
        {
            do_init(other);
        }
        template<class T, class U = decltype(type_matcher::match(std::declval<T>()))>
        content(T&& other) noexcept(std::is_nothrow_constructible<U, T>::value)
          : _which(switcher::index(detail::type<U>{}))
        {
            new(_storage) U(std::forward<T>(other));
        }
        ~content()
        {
            if (valid())
            {
                detail::dtor_visitor v;
                switcher::visit(_which, data(), v);
            }
        }
        template<class T, class U = decltype(type_matcher::match(std::declval<T>()))>
        U& operator=(T&& other) noexcept(detail::noexcept_ctor_assign<U, T>::value)
        {
            if (switcher::index(detail::type<U>{}) == _which)
                return *static_cast<U*>(data()) = std::forward<T>(other);
            else
            {
                invalidate();
                auto p = new(_storage) U(std::forward<T>(other));
                _which = switcher::index(detail::type<U>{});
                return *p;
            }
        }
        content& operator=(content&& other) noexcept(true)
        {
            do_assign(other);
            return *this;
        }
        content& operator=(content const& other)
        {
            do_assign(other);
            return *this;
        }

        content() noexcept : _which(0) {}
    };
#undef BUSTACHE_AST_CONTENT

    inline bool is_null(content const& c)
    {
        return !c.which();
    }
}}

#endif