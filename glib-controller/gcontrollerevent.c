/**
 * SECTION:gcontrollerevent
 * @Title: GControllerEvent
 * @Short_Description: A event to a storage change
 *
 * The #GControllerEvent is an object created by a #GController whenever
 * a controlled data storage changes. The #GControllerEvent stores the
 * the location of the changed data in a way that allows a View to query the
 * storage for the actual data.
 *
 * A #GControllerEvent can only be created by a #GController using
 * g_controller_create_event() and should be passed to the
 * #GController::changed signal emitter, g_controller_emit_changed()
 */

#include "config.h"

#include "gcontrollerevent.h"
#include "gcontroller.h"
#include "gcontrollerenumtypes.h"
#include "gcontrollertypes.h"

#include <string.h>
#include <gobject/gvaluecollector.h>

struct _GControllerEventPrivate
{
  GController *controller;

  GControllerAction action;

  GType index_type;
  GValueArray *indices;
};

enum
{
  PROP_0,

  PROP_CONTROLLER,
  PROP_ACTION,
  PROP_INDEX_TYPE,
  PROP_INDICES
};

G_DEFINE_TYPE (GControllerEvent,
               g_controller_event,
               G_TYPE_OBJECT);

static GValueArray *
add_indices (GValueArray *cur_indices,
             GValueArray *new_indices)
{
  gint i;

  if (new_indices == NULL)
    return cur_indices;

  if (cur_indices == NULL)
    cur_indices = g_value_array_new (new_indices->n_values);

  for (i = 0; i < new_indices->n_values; i++)
    g_value_array_append (cur_indices, g_value_array_get_nth (new_indices, i));

  return cur_indices;
}

static void
g_controller_event_constructed (GObject *gobject)
{
  GControllerEventPrivate *priv = G_CONTROLLER_EVENT (gobject)->priv;

  g_assert (G_IS_CONTROLLER (priv->controller));

  if (priv->action == G_CONTROLLER_INVALID_ACTION)
    {
      g_critical ("The constructed event for the GController "
                  "of type '%s' does not have a valid action.",
                  G_OBJECT_TYPE_NAME (priv->controller));
    }

  if (priv->index_type == G_TYPE_INVALID)
    {
      g_critical ("The constructed event for the GController "
                  "of type '%s' does not have a valid index type.",
                  G_OBJECT_TYPE_NAME (priv->controller));
    }
}

static void
g_controller_event_set_property (GObject      *gobject,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GControllerEventPrivate *priv = G_CONTROLLER_EVENT (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CONTROLLER:
      priv->controller = g_object_ref (g_value_get_object (value));
      break;

    case PROP_ACTION:
      priv->action = g_value_get_enum (value);
      break;

    case PROP_INDEX_TYPE:
      priv->index_type = g_value_get_gtype (value);
      break;

    case PROP_INDICES:
      priv->indices = add_indices (priv->indices, g_value_get_boxed (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
g_controller_event_get_property (GObject    *gobject,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GControllerEventPrivate *priv = G_CONTROLLER_EVENT (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CONTROLLER:
      g_value_set_object (value, priv->controller);
      break;

    case PROP_ACTION:
      g_value_set_enum (value, priv->action);
      break;

    case PROP_INDEX_TYPE:
      g_value_set_gtype (value, priv->index_type);
      break;

    case PROP_INDICES:
      g_value_set_boxed (value, priv->indices);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
g_controller_event_dispose (GObject *gobject)
{
  GControllerEventPrivate *priv = G_CONTROLLER_EVENT (gobject)->priv;

  if (priv->controller != NULL)
    {
      g_object_unref (priv->controller);
      priv->controller = NULL;
    }

  G_OBJECT_CLASS (g_controller_event_parent_class)->finalize (gobject);
}

static void
g_controller_event_finalize (GObject *gobject)
{
  GControllerEventPrivate *priv = G_CONTROLLER_EVENT (gobject)->priv;

  if (priv->indices != NULL)
    g_value_array_free (priv->indices);

  G_OBJECT_CLASS (g_controller_event_parent_class)->finalize (gobject);
}

static void
g_controller_event_class_init (GControllerEventClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  g_type_class_add_private (klass, sizeof (GControllerEventPrivate));

  gobject_class->constructed = g_controller_event_constructed;
  gobject_class->set_property = g_controller_event_set_property;
  gobject_class->get_property = g_controller_event_get_property;
  gobject_class->dispose = g_controller_event_dispose;
  gobject_class->finalize = g_controller_event_finalize;

  /**
   * GControllerEvent:controller:
   *
   * The #GController instance that created this event
   */
  pspec = g_param_spec_object ("controller",
                               "Controller",
                               "The controller instance that created the event",
                               G_TYPE_CONTROLLER,
                               G_PARAM_READWRITE |
                               G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (gobject_class, PROP_CONTROLLER, pspec);

  /**
   * GControllerEvent:action:
   *
   * The #GControllerAction that caused the creation of the event
   */
  pspec = g_param_spec_enum ("action",
                             "Action",
                             "The action that caused the creation of the event",
                             G_TYPE_CONTROLLER_ACTION,
                             G_CONTROLLER_INVALID_ACTION,
                             G_PARAM_READWRITE |
                             G_PARAM_CONSTRUCT_ONLY |
                             G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (gobject_class, PROP_ACTION, pspec);

  /**
   * GControllerEvent:index-type:
   *
   * The #GType representation of an index stored by the event
   */
  pspec = g_param_spec_gtype ("index-type",
                              "Index Type",
                              "The type of the indices",
                              G_TYPE_NONE,
                              G_PARAM_READWRITE |
                              G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (gobject_class, PROP_INDEX_TYPE, pspec);

  /**
   * GControllerEvent:indices:
   *
   * A #GValueArray containing all the indices stored by the event
   *
   * The indices are meaningful only for the data storage controlled
   * by the #GController that created this event
   */
  pspec = g_param_spec_boxed ("indices",
                              "Indices",
                              "The indices inside the data storage",
                              G_TYPE_VALUE_ARRAY,
                              G_PARAM_READWRITE |
                              G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (gobject_class, PROP_INDICES, pspec);
}

static void
g_controller_event_init (GControllerEvent *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            G_TYPE_CONTROLLER_EVENT,
                                            GControllerEventPrivate);

  self->priv->controller = NULL;

  self->priv->action = G_CONTROLLER_INVALID_ACTION;

  self->priv->index_type = G_TYPE_INVALID;
  self->priv->indices = NULL;
}

/**
 * g_controller_event_get_n_indices:
 * @event: a #GControllerEvent
 *
 * Retrieves the number of indices stored by the @event
 *
 * Return value: the number of indices
 */
gint
g_controller_event_get_n_indices (GControllerEvent *event)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), 0);

  if (event->priv->indices == NULL)
    return 0;

  return event->priv->indices->n_values;
}

/**
 * g_controller_event_get_index_type:
 * @event: a #GControllerEvent
 *
 * Retrieves the type of the indices stored by the @event
 *
 * Return value: a #GType
 */
GType
g_controller_event_get_index_type (GControllerEvent *event)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), G_TYPE_INVALID);

  return event->priv->index_type;
}

/**
 * g_controller_event_get_controller:
 * @event: a #GControllerEvent
 *
 * Retrieves a pointer to the #GController that created this event
 *
 * Return value: (transfer none): a pointer to a #GController
 */
GController *
g_controller_event_get_controller (GControllerEvent *event)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), NULL);

  return event->priv->controller;
}

/**
 * g_controller_event_get_action:
 * @event: a #GControllerEvent
 *
 * Retrieves the action that caused the creation of this event
 *
 * Return value: a #GControllerAction
 */
GControllerAction
g_controller_event_get_action (GControllerEvent *event)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), G_CONTROLLER_INVALID_ACTION);

  return event->priv->action;
}

/**
 * g_controller_event_add_index_value:
 * @event: a #GControllerEvent
 * @value: a #GValue containing an index
 *
 * Adds an index stored inside a #GValue.
 *
 * The #GValue must contain a value with the same type as the
 * #GControllerEvent:index-type
 *
 * This function is mostly useful for bindings
 */
void
g_controller_event_add_index_value (GControllerEvent *event,
                                    const GValue     *value)
{
  g_return_if_fail (G_IS_CONTROLLER_EVENT (event));
  g_return_if_fail (G_VALUE_TYPE (value) == event->priv->index_type);

  if (event->priv->indices == NULL)
    event->priv->indices = g_value_array_new (0);

  g_value_array_append (event->priv->indices, value);
}

/**
 * g_controller_event_get_index_value:
 * @event: a #GControllerEvent
 * @index_: the position of the index, between 0 and the number of
 *   indices stored by @event
 * @value: an uninitialized #GValue
 *
 * Retrieves the index at @index_ and stores it inside @value
 *
 * Return value: %TRUE on success
 */
gboolean
g_controller_event_get_index_value (GControllerEvent *event,
                                    gint              index_,
                                    GValue           *value)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  if (event->priv->indices == NULL)
    return FALSE;

  if (index_ < 0 || index_ >= event->priv->indices->n_values)
    return FALSE;

  g_value_init (value, event->priv->index_type);
  g_value_copy (g_value_array_get_nth (event->priv->indices, index_), value);

  return TRUE;
}

/**
 * g_controller_event_add_index:
 * @event: a #GControllerEvent
 * @...: the index to add
 *
 * Variadic arguments version of g_controller_event_add_index_value(),
 * for the convenience of users of the C API
 */
void
g_controller_event_add_index (GControllerEvent *event,
                              ...)
{
  GValue value = { 0, };
  gchar *error = NULL;
  va_list args;

  g_return_if_fail (G_IS_CONTROLLER_EVENT (event));

  va_start (args, event);

  if (event->priv->indices == NULL)
    event->priv->indices = g_value_array_new (1);

  G_VALUE_COLLECT_INIT (&value, event->priv->index_type, args, 0, &error);

  if (error != NULL)
    {
      g_warning ("%s: %s", G_STRLOC, error);
      g_free (error);
      goto out;
    }

  event->priv->indices = g_value_array_append (event->priv->indices, &value);
  g_value_unset (&value);

out:
  va_end (args);
}

/**
 * g_controller_event_get_index:
 * @event: a #GControllerEvent
 * @index_: the position of the index
 * @...: return location for the index value at the given index_ition
 *
 * Retrieves the index inside the @event
 *
 * Return value: %TRUE on success
 */
gboolean
g_controller_event_get_index (GControllerEvent *event,
                              gint              index_,
                              ...)
{
  GValue *value;
  gchar *error = NULL;
  va_list args;
  gboolean res = FALSE;

  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), FALSE);

  if (event->priv->indices == NULL)
    return FALSE;

  va_start (args, index_);

  value = g_value_array_get_nth (event->priv->indices, index_);
  if (value == NULL)
    goto out;

  G_VALUE_LCOPY (value, args, 0, &error);

  if (error != NULL)
    {
      g_warning ("%s: %s", G_STRLOC, error);
      g_free (error);
      res = FALSE;
    }
  else
    res = TRUE;

out:
  va_end (args);

  return res;
}

/**
 * g_controller_event_get_index_int:
 * @event: a #GControllerEvent
 * @index_: the position of the index
 *
 * Typed accessor for integer indexes stored inside the @event
 *
 * Return value: an integer index at the given index_ition
 */
gint
g_controller_event_get_index_int (GControllerEvent *event,
                                  gint              index_)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), 0);
  g_return_val_if_fail (event->priv->index_type == G_TYPE_INT, 0);

  if (event->priv->indices == NULL)
    return 0;

  if (index_ < 0 || index_ >= event->priv->indices->n_values)
    return 0;

  return g_value_get_int (g_value_array_get_nth (event->priv->indices, index_));
}

/**
 * g_controller_event_get_index_uint:
 * @event: a #GControllerEvent
 * @index_: the position of the index
 *
 * Typed accessor for unsigned integer indexes stored inside the @event
 *
 * Return value: an unsigned integer index at the given index_ition
 */
guint
g_controller_event_get_index_uint (GControllerEvent *event,
                                   gint              index_)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), 0);
  g_return_val_if_fail (event->priv->index_type == G_TYPE_UINT, 0);

  if (event->priv->indices == NULL)
    return 0;

  if (index_ < 0 || index_ >= event->priv->indices->n_values)
    return 0;

  return g_value_get_uint (g_value_array_get_nth (event->priv->indices, index_));
}

/**
 * g_controller_event_get_index_string:
 * @event: a #GControllerEvent
 * @index_: the position of the index
 *
 * Typed accessor for string indexes stored inside the @event
 *
 * Return value: a string index at the given index_ition
 */
const gchar *
g_controller_event_get_index_string (GControllerEvent *event,
                                     gint              index_)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), NULL);
  g_return_val_if_fail (event->priv->index_type == G_TYPE_STRING, NULL);

  if (event->priv->indices == NULL)
    return 0;

  if (index_ < 0 || index_ >= event->priv->indices->n_values)
    return 0;

  return g_value_get_string (g_value_array_get_nth (event->priv->indices, index_));
}

/**
 * g_controller_event_get_index_pointer:
 * @event: a #GControllerEvent
 * @index_: the position of the index
 *
 * Typed accessor for pointer indexes stored inside the @event
 *
 * Return value: (transfer none): a pointer index at the given index_ition
 */
gpointer
g_controller_event_get_index_pointer (GControllerEvent *event,
                                      gint              index_)
{
  g_return_val_if_fail (G_IS_CONTROLLER_EVENT (event), NULL);
  g_return_val_if_fail (event->priv->index_type == G_TYPE_POINTER, NULL);

  if (event->priv->indices == NULL)
    return 0;

  if (index_ < 0 || index_ >= event->priv->indices->n_values)
    return 0;

  return g_value_get_pointer (g_value_array_get_nth (event->priv->indices, index_));
}
