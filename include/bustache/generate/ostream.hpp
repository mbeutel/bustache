/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2016-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BUSTACHE_GENERATE_OSTREAM_HPP_INCLUDED
#define BUSTACHE_GENERATE_OSTREAM_HPP_INCLUDED

#include <iostream>
#include <bustache/generate.hpp>

namespace bustache { namespace detail
{
    template<class CharT, class Traits>
    struct ostream_sink
    {
        std::basic_ostream<CharT, Traits>& out;

        void operator()(char const* it, char const* end) const
        {
            out.write(it, end - it);
        }

        template<class T>
        void operator()(T data) const
        {
            out << data;
        }

        void operator()(bool data) const
        {
            out << (data ? "true" : "false");
        }
    };
}}

namespace bustache
{
    template<class CharT, class Traits, class Context, class UnresolvedHandler>
    void generate_ostream
    (
        std::basic_ostream<CharT, Traits>& out, format const& fmt,
        value_view const& data, Context const& context,
        option_type flag, UnresolvedHandler&& f
    )
    {
        detail::ostream_sink<CharT, Traits> sink{out};
        generate(sink, fmt, data, context, flag, std::forward<UnresolvedHandler>(f));
    }

    // This is instantiated in src/generate.cpp.
    extern template
    void generate_ostream
    (
        std::ostream& out, format const& fmt,
        value_view const& data, context_view const& context,
        option_type flag, default_unresolved_handler&&
    );
}

#endif