#pragma once

/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <halp/log.hpp>
#include <halp/messages.hpp>
#include <halp/meta.hpp>

#include <cstdio>
// Note: we use a generic logger abstraction here, so
// that we can use e.g. Pd / Max's post in their case, and
// printf / fmt::print otherwise.
// See Logger.hpp for more detail on how that works !

namespace examples::helpers
{
inline void free_example()
{
  printf("free_example A\n");
  fflush(stdout);
}

template <typename C>
inline void free_template_example()
{
  using logger = typename C::logger_type;
  logger{}.debug("free_example");
}

// See Logger.hpp
template <typename C>
requires halp::has_logger<C>
struct Messages
{
  halp_meta(name, "Message helpers")
  halp_meta(c_name, "avnd_helpers_messages")
  halp_meta(uuid, "0029b546-cddb-49b1-9c99-c659b16e58eb")

  [[no_unique_address]] typename C::logger_type logger;

  void example() { logger.info("example"); }

  void example2(float x) { logger.error("example2: {}", x); }

  template <typename U>
  void example3(U x)
  {
    logger.warn("example3: {}", x);
  }

  halp_start_messages(Messages) halp_mem_fun(example)
  halp_mem_fun(example2)
  halp_mem_fun_t(example3, <int>)
  halp_mem_fun_t(example3, <float>)
  halp_mem_fun_t(example3, <const char*>)
  halp_free_fun(free_example)
  halp_free_fun_t(free_template_example, <C>)
  halp_lambda(my_lambda, [](Messages& self) { puts("lambda"); })

  // General case:
  halp::func_ref<"my_message", &Messages::example> m_my_message;
  halp_end_messages
};
}
