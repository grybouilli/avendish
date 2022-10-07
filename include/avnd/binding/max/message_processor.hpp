#pragma once

/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <avnd/binding/max/helpers.hpp>
#include <avnd/binding/max/init.hpp>
#include <avnd/binding/max/inputs.hpp>
#include <avnd/binding/max/messages.hpp>
#include <avnd/binding/max/outputs.hpp>
#include <avnd/common/export.hpp>
#include <avnd/wrappers/avnd.hpp>
#include <avnd/wrappers/controls.hpp>
#include <cmath>

#include <span>
#include <string>

/**
 * This Max processor is used when there is no dsp processing involved.
 *
 * It will create one inlet per input port.
 */
namespace max
{
template <typename T>
struct message_processor_metaclass
{
  static inline t_class* g_class{};
  static inline message_processor_metaclass* instance{};

  static t_symbol* symbol_from_name();
  message_processor_metaclass();
};

template <typename T>
struct message_processor
{
  // Head of the Max object
  t_object x_obj;

  // Our actual code
  avnd::effect_container<T> implementation;

  // Setup, storage...for the outputs
  [[no_unique_address]] inputs<T> input_setup;

  [[no_unique_address]] outputs<T> output_setup;

  [[no_unique_address]] init_arguments<T> init_setup;

  [[no_unique_address]] messages<T> messages_setup;

  // we don't use ctor / dtor, because
  // this breaks aggregate-ness...
  void init(int argc, t_atom* argv)
  {
    /// Pass arguments
    if constexpr(avnd::can_initialize<T>)
    {
      init_setup.process(implementation.effect, argc, argv);
    }

    /// Create ports ///
    // Create inlets
    input_setup.init(implementation, x_obj);

    // Create outlets
    output_setup.init(implementation, x_obj);

    /// Initialize controls
    if constexpr(avnd::has_inputs<T>)
    {
      avnd::init_controls(implementation);
    }
  }

  void destroy() { }

  void process_inlet_control(int inlet, t_atom_long val)
  {
    if constexpr(avnd::has_inputs<T>)
    {
      avnd::input_introspection<T>::for_nth(
          avnd::get_inputs<T>(implementation), inlet, [val](auto& field) {
            // TODO dangerous if const char*
            if constexpr(requires { field.value = 0; })
            {
              field.value = val;
            }
          });
    }
  }

  void process_inlet_control(int inlet, t_atom_float val)
  {
    if constexpr(avnd::has_inputs<T>)
    {
      avnd::input_introspection<T>::for_nth(
          avnd::get_inputs<T>(implementation), inlet, [val](auto& field) {
            // TODO dangerous if const char*
            if constexpr(requires { field.value = 0; })
            {
              field.value = val;
            }
          });
    }
  }

  void process_inlet_control(int inlet, struct symbol* val)
  {
    if constexpr(avnd::has_inputs<T>)
    {
      avnd::input_introspection<T>::for_nth(
          avnd::get_inputs<T>(implementation), inlet, [val](auto& field) {
            // TODO dangerous if const char*
            if constexpr(requires { field.value = "str"; })
            {
              field.value = val->s_name;
            }
          });
    }
  }

  void process_inlet_control(int inlet, t_symbol* s, int argc, t_atom* argv)
  {
    if constexpr(avnd::has_inputs<T>)
    {
      switch(argv[0].a_type)
      {
        case A_LONG:
          process_inlet_control(inlet, argv[0].a_w.w_long);
          break;

        case A_FLOAT:
          process_inlet_control(inlet, argv[0].a_w.w_float);
          break;

        case A_SYM:
          process_inlet_control(inlet, argv[0].a_w.w_sym);
          break;

        default:
          break;
      }
    }
  }

  void process()
  {
    // Do our stuff if it makes sense - some objects may not
    // even have a "processing" method
    if_possible(implementation.effect());

    // Then bang
    output_setup.commit(implementation);
  }

  template <typename V>
  void process(V value)
  {
    const int inlet = proxy_getinlet(&x_obj);
    // Process the control
    process_inlet_control(inlet, value);

    process();
  }

  void process(t_symbol* s, int argc, t_atom* argv)
  {
    const int inlet = proxy_getinlet(&x_obj);

    // First try to process messages handled explicitely in the object
    if(inlet == 0 && messages_setup.process_messages(implementation, s, argc, argv))
      return;

    // Then some default behaviour
    switch(argc)
    {
      case 0: // bang
      {
        if(inlet == 0)
        {
          if(strcmp(s->s_name, "bang") == 0)
          {
            process();
          }
          else
          {
            process_generic_message(implementation, s);
          }
          break;
        }
        [[fallthrough]];
      }
      default: {
        // First apply the data to the first inlet (other inlets are handled by Pd).
        process_inlet_control(inlet, s, argc, argv);

        process();
        break;
      }
    }
  }
};

template <typename T>
message_processor_metaclass<T>::message_processor_metaclass()
{
  message_processor_metaclass::instance = this;
  using instance = message_processor<T>;
#if !defined(_MSC_VER)
  static_assert(std::is_aggregate_v<T>);
  static_assert(std::is_aggregate_v<instance>);
  static_assert(std::is_nothrow_constructible_v<instance>);
  static_assert(std::is_nothrow_move_constructible_v<instance>);
  static_assert(std::is_nothrow_move_assignable_v<instance>);
#endif
  /// Small wrapper methods which will call into our actual type ///

  // Ctor
  constexpr auto obj_new = +[](t_symbol* s, int argc, t_atom* argv) -> void* {
    // Initializes the t_object
    auto* ptr = object_alloc(g_class);

    // Initializes the rest
    auto obj = reinterpret_cast<instance*>(ptr);
    new(obj) instance;
    obj->init(argc, argv);
    return obj;
  };

  // Dtor
  constexpr auto obj_free = +[](instance* obj) -> void {
    obj->destroy();
    obj->~instance();
  };

  // Message processing
  constexpr auto obj_process
      = +[](instance* obj, t_symbol* s, int argc, t_atom* argv) -> void {
    obj->process(s, argc, argv);
  };

  constexpr auto obj_process_bang = +[](instance* obj) -> void { obj->process(); };

  constexpr auto obj_process_int
      = +[](instance* obj, t_atom_long value) -> void { obj->process(value); };

  constexpr auto obj_process_float
      = +[](instance* obj, t_atom_float value) -> void { obj->process(value); };

  constexpr auto obj_process_sym
      = +[](instance* obj, t_symbol* value) -> void { obj->process(value); };

  /// Class creation ///
  g_class = class_new(
      avnd::get_c_name<T>().data(), (method)obj_new, (method)obj_free,
      sizeof(message_processor<T>), 0L, A_GIMME, 0);

  // Connect our methods
  class_addmethod(g_class, (method)obj_process_int, "int", A_LONG, 0);
  class_addmethod(g_class, (method)obj_process_float, "float", A_FLOAT, 0);
  class_addmethod(g_class, (method)obj_process_sym, "symbol", A_SYM, 0);
  class_addmethod(g_class, (method)obj_process_bang, "bang", A_NOTHING, 0);
  class_addmethod(g_class, (method)obj_process, "anything", A_GIMME, 0);

  class_register(CLASS_BOX, g_class);
}

}

#define PD_DEFINE_EFFECT(EffectCName, EffectMainClass)                        \
  extern "C" AVND_EXPORTED_SYMBOL void EffectCName##_setup()                  \
  {                                                                           \
    static const pd::message_processor_metaclass<EffectMainClass> instance{}; \
  }
