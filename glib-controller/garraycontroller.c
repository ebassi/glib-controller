#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "garraycontroller.h"

/**
 * SECTION:garraycontroller
 * @Title: GArrayController
 * @Short_Description: A controller for GLib arrays
 * @See_Also: #GController, #GArray
 *
 * The #GArrayController object is a #GController implementation aimed
 * at controlling #GArray data structures
 */

struct _GArrayControllerPrivate
{
  GArray *array;
};

enum
{
  PROP_0,

  ARRAY,

  N_PROPERTIES
};

static GParamSpec *controller_props[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE (GArrayController, g_array_controller, G_TYPE_CONTROLLER)

static void
g_array_controller_set_property (GObject      *gobject,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GArrayController *self = G_ARRAY_CONTROLLER (gobject);

  switch (prop_id)
    {
    case ARRAY:
      g_array_controller_set_array (self, g_value_get_boxed (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
g_array_controller_get_property (GObject    *gobject,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GArrayControllerPrivate *priv = G_ARRAY_CONTROLLER (gobject)->priv;

  switch (prop_id)
    {
    case ARRAY:
      g_value_set_boxed (value, priv->array);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
g_array_controller_dispose (GObject *gobject)
{
  GArrayControllerPrivate *priv = G_ARRAY_CONTROLLER (gobject)->priv;

  if (priv->array != NULL)
    {
      g_array_unref (priv->array);
      priv->array = NULL;
    }

  G_OBJECT_CLASS (g_array_controller_parent_class)->dispose (gobject);
}

static void
g_array_controller_class_init (GArrayControllerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = g_array_controller_set_property;
  gobject_class->get_property = g_array_controller_get_property;
  gobject_class->dispose = g_array_controller_dispose;

  /**
   * GArrayController:array:
   *
   * The #GArray to be controlled by a #GArrayController instance
   */
  controller_props[ARRAY] =
    g_param_spec_boxed ("array",
                        "Array",
                        "The GArray to be controlled",
                        G_TYPE_ARRAY,
                        G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT |
                        G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPERTIES, controller_props);

  g_type_class_add_private (klass, sizeof (GArrayControllerPrivate));
}

static void
g_array_controller_init (GArrayController *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            G_TYPE_ARRAY_CONTROLLER,
                                            GArrayControllerPrivate);
}

/**
 * g_array_controller_new:
 *
 * Creates a new #GArrayController.
 *
 * Return value: (transfer full): the newly created #GArrayController
 */
GController *
g_array_controller_new (void)
{
  return g_object_new (G_TYPE_ARRAY_CONTROLLER, NULL);
}

/**
 * g_array_controller_new_with_array:
 * @array: (allow-none): a #GArray or %NULL
 *
 * Creates a new #GArrayController controlling the @array.
 *
 * Return value: (transfer full): the newly created #GArrayController
 */
GController *
g_array_controller_new_with_array (GArray *array)
{
  return g_object_new (G_TYPE_ARRAY_CONTROLLER,
                       "array", array,
                       NULL);
}

/**
 * g_array_controller_set_array:
 * @controller: a #GArrayController
 * @array: (allow-none): a #GArray or %NULL
 *
 * Sets the #GArray to be controlled by @controller
 *
 * The #GArrayController will take a reference on the passed #GArray
 * which will be released when the #GArrayController is disposed or
 * when unsetting the controlled array by passing %NULL to this
 * function
 */
void
g_array_controller_set_array (GArrayController *controller,
                              GArray           *array)
{
  g_return_if_fail (G_IS_ARRAY_CONTROLLER (controller));

  if (controller->priv->array == array)
    return;

  if (controller->priv->array != NULL)
    g_array_unref (controller->priv->array);

  controller->priv->array = array;

  if (controller->priv->array != NULL)
    g_array_ref (controller->priv->array);

  g_object_notify_by_pspec (G_OBJECT (controller), controller_props[ARRAY]);
}

/**
 * g_array_controller_get_array:
 * @controller: a #GArrayController
 *
 * Retrieves the #GArray controlled by @controller
 *
 * Return value: (transfer none): a pointer to a #GArray, or %NULL
 */
GArray *
g_array_controller_get_array (GArrayController *controller)
{
  g_return_val_if_fail (G_IS_ARRAY_CONTROLLER (controller), NULL);

  return controller->priv->array;
}
