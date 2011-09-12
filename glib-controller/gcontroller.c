/**
 * SECTION:gcontroller
 * @Title: GController
 * @Short_Description: A controller for data storage changes
 *
 * #GController is an abstract class for creating controllers (in the
 * "model-view-controller" pattern sense) for GLib data types.
 *
 * A #GController is associated to a data storage, and whenever the
 * storage changes it will emit the #GController::changed signal,
 * detailing:
 *
 * <itemizedlist>
 *   <listitem><simpara>what kind of change it is</simpara></listitem>
 *   <listitem><simpara>where to access the changed data</simpara></listitem>
 * </itemizedlist>
 */

#include "config.h"

#include "gcontroller.h"
#include "gcontrollerenumtypes.h"
#include "gcontrollermarshal.h"
#include "gcontrollerevent.h"

#include <string.h>
#include <gobject/gvaluecollector.h>

enum
{
  CHANGED,

  LAST_SIGNAL
};

static guint controller_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_ABSTRACT_TYPE (GController, g_controller, G_TYPE_OBJECT);

static GControllerEvent *
create_event (GController       *controller,
              GControllerAction  action,
              GType              index_type,
              GValueArray       *indices)
{
  g_assert (index_type != G_TYPE_INVALID);

  return g_object_new (G_TYPE_CONTROLLER_EVENT,
                       "controller", controller,
                       "action", action,
                       "index-type", index_type,
                       "indices", indices,
                       NULL);
}

static void
g_controller_class_init (GControllerClass *klass)
{
  klass->create_event = create_event;

  /**
   * GController::changed:
   * @controller: the #GController that emitted the signal
   * @action: the action on the data storage controlled by @controller
   * @event: a reference to the indices changed
   *
   * The ::changed signal is emitted each time the data storage controlled
   * by a #GController instance changes. The type of change is detailed by
   * the #GControllerAction enumeration and passed as the @action argument
   * to the signal handlers.
   *
   * The @event object contains the information necessary for retrieving
   * the data that was affected by the change from the controlled storage;
   * the type of the information depends on the #GController.
   *
   * Views using a #GController to monitor changes inside a data storage
   * should connect to the #GController::changed signal and update themselves
   * whenever the signal is emitted.
   */
  controller_signals[CHANGED] =
    g_signal_new (g_intern_static_string ("changed"),
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GControllerClass, changed),
                  NULL, NULL,
                  _gcontroller_marshal_VOID__ENUM_OBJECT,
                  G_TYPE_NONE, 2,
                  G_TYPE_CONTROLLER_ACTION,
                  G_TYPE_CONTROLLER_EVENT);
}

static void
g_controller_init (GController *self)
{
}

static GControllerEvent *
g_controller_create_event_internal (GController       *controller,
                                    GControllerAction  action,
                                    GType              index_type,
                                    GValueArray       *indices)
{
  return G_CONTROLLER_GET_CLASS (controller)->create_event (controller,
                                                            action,
                                                            index_type,
                                                            indices);
}

/**
 * g_controller_create_eventv
 * @controller: a #GController
 * @action: the action for the event 
 * @index_type: the type of the indices
 * @indices: (allow-none): a #GValueArray containing the indices
 *
 * Creates a new #GControllerEvent for the given indices.
 *
 * This function should only be used by implementations of the
 * #GController API.
 *
 * The passed @indices array is copied inside the #GControllerEvent
 * and can be safely freed afterwards.
 *
 * If @indices is %NULL a new empty event will be created; it is
 * possible to add indexes to it by using g_controller_event_add_index()
 * or g_controller_event_add_index_value().
 *
 * This is a vector-based variant of g_controller_create_event(), for
 * the convenience of language bindings.
 *
 * Return value: (transfer full): a newly created #GControllerEvent
 *   instance. Use g_object_unref() when done using the returned
 *   object
 */
GControllerEvent *
g_controller_create_eventv (GController       *controller,
                            GControllerAction  action,
                            GType              index_type,
                            GValueArray       *indices)
{
  g_return_val_if_fail (G_IS_CONTROLLER (controller), NULL);
  g_return_val_if_fail (index_type != G_TYPE_INVALID, NULL);

  return g_controller_create_event_internal (controller, action,
                                             index_type,
                                             indices);
}

/**
 * g_controller_create_event:
 * @controller: a #GController
 * @action: the action for the event 
 * @index_type: the type of the indices
 * @n_indices: the number of indices, or 0 to create an empty event and
 *   use the #GControllerEvent API to fill it
 * @...: the indices
 *
 * Creates a new #GControllerEvent for the given indices.
 *
 * This function should only be used by implementations of the
 * #GController API.
 *
 * This is a variadic arguments version of g_controller_create_eventv(),
 * for the convenience of users of the C API.
 *
 * Return value: (transfer full): a newly created #GControllerEvent
 *   instance. Use g_object_unref() when done using the returned
 *   object
 */
GControllerEvent *
g_controller_create_event (GController       *controller,
                           GControllerAction  action,
                           GType              index_type,
                           gint               n_indices,
                           ...)
{
  GControllerEvent *ref;
  GValueArray *indices;
  va_list args;
  gint i;

  g_return_val_if_fail (G_IS_CONTROLLER (controller), NULL);
  g_return_val_if_fail (index_type != G_TYPE_INVALID, NULL);

  /* short-circuit the empty event case */
  if (n_indices == 0)
    return g_controller_create_event_internal (controller, action, index_type, NULL);

  indices = g_value_array_new (n_indices);

  va_start (args, n_indices);

  for (i = 0; i < n_indices; i++)
    {
      gchar *error;

      g_value_array_insert (indices, i, NULL);

      G_VALUE_COLLECT_INIT (&(indices->values[i]), index_type,
                            args, 0,
                            &error);

      if (error != NULL)
        {
          g_warning ("%s: %s", G_STRLOC, error);
          g_free (error);
          break;
        }
    }

  va_end (args);

  ref = g_controller_create_event_internal (controller, action,
                                            index_type,
                                            indices);

  g_value_array_free (indices);

  return ref;
}

/**
 * g_controller_emit_changed:
 * @controller: a #GController
 * @event: the reference to the changed data
 *
 * Emits the #GController::changed signal with the given
 * parameters
 */
void
g_controller_emit_changed (GController      *controller,
                           GControllerEvent *event)
{
  g_return_if_fail (G_IS_CONTROLLER (controller));
  g_return_if_fail (G_IS_CONTROLLER_EVENT (event));

  g_signal_emit (controller, controller_signals[CHANGED], 0,
                 g_controller_event_get_action (event),
                 event);
}
