#pragma once

/* SPDX-License-Identifier: GPL-3.0-or-later OR BSL-1.0 OR CC0-1.0 OR CC-PDCC OR 0BSD */

namespace avnd
{

template <typename T>
concept logger = requires(T t) {
#if defined(__APPLE__)
                   T{};
#else
#if defined(__clang__) && (__clang_major__ >= 12) && (__clang_major__ < 14)
                   &T::trace;
                   &T::info;
                   &T::debug;
                   &T::warn;
                   &T::error;
                   &T::critical;
#else
                   t.trace("{} {}", 1, "foo");
                   t.info("{} {}", 1, "foo");
                   t.debug("{} {}", 1, "foo");
                   t.warn("{} {}", 1, "foo");
                   t.error("{} {}", 1, "foo");
                   t.critical("{} {}", 1, "foo");
#endif
#endif
                 };

}
