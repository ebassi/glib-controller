#include "config.h"

#include "giterable.h"
#include "giterator.h"

G_DEFINE_INTERFACE (GIterable, g_iterable, G_TYPE_OBJECT)

static GIterator *
g_iterable_real_create_iterator (GIterable *iterable)
{
  g_warning ("The iterable of type '%s' does not implement the GIterable::create_iterator virtual function",
             G_OBJECT_TYPE_NAME (iterable));

  return NULL;
}

static void
g_iterable_default_init (GIterableInterface *iface)
{
  iface->create_iterator = g_iterable_real_create_iterator;
}

/**
 * g_iterable_create_iterator:
 * @iterable: FIXME
 *
 * FIXME
 *
 * Return value: (transfer full): FIXME
 */
GIterator *
g_iterable_create_iterator (GIterable *iterable)
{
  g_return_val_if_fail (G_IS_ITERABLE (iterable), NULL);

  return G_ITERABLE_GET_INTERFACE (iterable)->create_iterator (iterable);
}
