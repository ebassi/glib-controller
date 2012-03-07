#ifndef __G_ITERATOR_H__
#define __G_ITERATOR_H__

#include <glib-controller/gcontrollertypes.h>

G_BEGIN_DECLS

#define G_ITERATOR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_ITERATOR, GIteratorClass))
#define G_IS_ITERATOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_ITERATOR))
#define G_ITERATOR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_ITERATOR, GIteratorClass))

typedef struct _GIteratorClass          GIteratorClass;

/**
 * GIterator:
 *
 * Ref Func: g_iterator_ref
 * Unref Func: g_iterator_unref
 * Set Value Func: g_value_set_iterator
 * Get Value Func: g_value_get_iterator
 */
struct _GIterator
{
  /*< private >*/
  GTypeInstance parent_instance;

  volatile int ref_count;
};

/**
 * GIteratorClass:
 * @next:
 * @destroy:
 *
 */
struct _GIteratorClass
{
  /*< private >*/
  GTypeClass parent_class;

  /*< public >*/
  gboolean (* next) (GIterator *iter);

  void (* destroy) (GIterator *iter);

  /*< private >*/
  gpointer _padding[2];
};

GType g_iterator_get_type (void) G_GNUC_CONST;

gpointer        g_iterator_create       (GType      iter_type);

gpointer        g_iterator_ref          (gpointer   iter);
void            g_iterator_unref        (gpointer   iter);

gboolean        g_iterator_next         (gpointer   iter);

#define G_VALUE_HOLDS_ITERATOR(v)       (G_VALUE_HOLDS (v, G_TYPE_ITERATOR))

void            g_value_set_iterator    (GValue       *value,
                                         gpointer      iter);
void            g_value_take_iterator   (GValue       *value,
                                         gpointer      iter);
gpointer        g_value_get_iterator    (const GValue *value);
gpointer        g_value_dup_iterator    (const GValue *value);

G_END_DECLS

#endif /* __G_ITERATOR_H__ */
