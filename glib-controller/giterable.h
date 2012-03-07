#ifndef __G_ITERABLE_H__
#define __G_ITERABLE_H__

#include <glib-controller/gcontrollertypes.h>

G_BEGIN_DECLS

#define G_ITERABLE_GET_INTERFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), G_TYPE_ITERABLE, GIterableInterface))

typedef struct _GIterableInterface      GIterableInterface;

struct _GIterableInterface
{
  GTypeInterface g_iface;

  GIterator *(* create_iterator) (GIterable *iterable);
};

GType g_iterable_get_type (void) G_GNUC_CONST;

GIterator *     g_iterable_create_iterator      (GIterable *iterable);

G_END_DECLS

#endif /* __G_ITERABLE_H__ */
