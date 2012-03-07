#include "config.h"

#include "giterable.h"
#include "giterator.h"

G_DEFINE_INTERFACE (GIterable, g_iterable, G_TYPE_OBJECT)

static GIterator *
g_iterable_real_create_iterator (GIterable *iterable)
{
  GIterableInterface *iface = G_ITERABLE_GET_INTERFACE (iterable);

  if (iface->iterator_type != G_TYPE_INVALID &&
      g_type_is_a (iface->iterator_type, G_TYPE_ITERATOR))
    return g_iterator_create (iface->iterator_type);

  return NULL;
}

static void
g_iterable_default_init (GIterableInterface *iface)
{
  iface->iterator_type = G_TYPE_INVALID;
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
