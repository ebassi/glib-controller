#include "config.h"

#include "giterator.h"

#include <gobject/gvaluecollector.h>

static void
value_iter_init_value (GValue *value)
{
  value->data[0].v_pointer = NULL;
}

static void
value_iter_free_value (GValue *value)
{
  if (value->data[0].v_pointer != NULL)
    g_iterator_unref (value->data[0].v_pointer);
}

static void
value_iter_copy_value (const GValue *src,
                       GValue       *dst)
{
  if (src->data[0].v_pointer != NULL)
    dst->data[0].v_pointer = g_iterator_ref (src->data[0].v_pointer);
  else
    dst->data[0].v_pointer = NULL;
}

static gpointer
value_iter_peek_pointer (const GValue *value)
{
  return value->data[0].v_pointer;
}

static gchar *
value_iter_collect_value (GValue *value,
                          guint n_collect_values,
                          GTypeCValue *collect_values,
                          guint collect_flags)
{
  GIterator *iter;

  iter = collect_values[0].v_pointer;

  if (iter == NULL)
    {
      value->data[0].v_pointer = NULL;
      return NULL;
    }

  if (iter->parent_instance.g_class == NULL)
    return g_strconcat ("invalid unclassed GIterator pointer for value type '",
                        G_VALUE_TYPE_NAME (value),
                        "'",
                        NULL);

  value->data[0].v_pointer = g_iterator_ref (iter);

  return NULL;
}

static gchar *
value_iter_lcopy_value (const GValue *value,
                        guint n_collect_values,
                        GTypeCValue *collect_values,
                        guint collect_flags)
{
  GIterator **iter_p = collect_values[0].v_pointer;

  if (iter_p == NULL)
    return g_strconcat ("value location for '",
                        G_VALUE_TYPE_NAME (value),
                        "' passed as NULL",
                        NULL);

  if (value->data[0].v_pointer == NULL)
    *iter_p = NULL;
  else if (collect_flags & G_VALUE_NOCOPY_CONTENTS)
    *iter_p = value->data[0].v_pointer;
  else
    *iter_p = g_iterator_ref (value->data[0].v_pointer);

  return NULL;
}

static const GTypeValueTable value_table = {
  value_iter_init_value,
  value_iter_free_value,
  value_iter_copy_value,
  value_iter_peek_pointer,
  (char *) "p",
  value_iter_collect_value,
  (char *) "p",
  value_iter_lcopy_value,
};

static void
g_iterator_class_base_finalize (GIteratorClass *klass)
{
}

static void
g_iterator_class_base_init (GIteratorClass *klass)
{
}

static gboolean
g_iterator_real_next (GIterator *iterator)
{
  g_warning ("The iterator of type '%s' does not implement the GIterator::next virtual function",
             g_type_name (G_TYPE_FROM_INSTANCE (iterator)));

  return FALSE;
}

static void
g_iterator_real_destroy (GIterator *iterator)
{
}

static void
g_iterator_class_init (GIteratorClass *klass)
{
  klass->next = g_iterator_real_next;
  klass->destroy = g_iterator_real_destroy;
}

static void
g_iterator_init (GIterator *iter)
{
  iter->ref_count = 1;
}

static const GTypeFundamentalInfo fundamental_info = {
  (G_TYPE_FLAG_CLASSED |
   G_TYPE_FLAG_INSTANTIATABLE |
   G_TYPE_FLAG_DERIVABLE |
   G_TYPE_FLAG_DEEP_DERIVABLE),
};

GType
g_iterator_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      const GTypeInfo iter_info = {
        sizeof (GIteratorClass),

        (GBaseInitFunc) g_iterator_class_base_init,
        (GBaseFinalizeFunc) g_iterator_class_base_finalize,
        (GClassInitFunc) g_iterator_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,

        sizeof (GIterator),
        0,

        (GInstanceInitFunc) g_iterator_init,

        &value_table,
      };

      GType g_define_type_id =
        g_type_register_fundamental (g_type_fundamental_next (),
                                     g_intern_static_string ("GIterator"),
                                     &iter_info,
                                     &fundamental_info,
                                     G_TYPE_FLAG_ABSTRACT);

      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

/**
 * g_iterator_create:
 * @iter_type: FIXME
 *
 * FIXME
 *
 * Return value: (type GIterator) (transfer full): FIXME
 */
gpointer
g_iterator_create (GType iter_type)
{
  g_return_val_if_fail (g_type_is_a (iter_type, G_TYPE_ITERATOR), NULL);

  return (gpointer) g_type_create_instance (iter_type);
}

/**
 * g_iterator_ref:
 * @iter: (type GIterator): FIXME
 *
 * FIXME
 *
 * Return value: (transfer none): FIXME
 */
gpointer
g_iterator_ref (gpointer iter)
{
  GIterator *iterator = iter;

  g_return_val_if_fail (G_IS_ITERATOR (iter), NULL);

  g_atomic_int_inc (&iterator->ref_count);

  return iter;
}

/**
 * g_iterator_unref:
 * @iter: FIXME
 *
 * FIXME
 */
void
g_iterator_unref (gpointer iter)
{
  GIterator *iterator = iter;

  g_return_if_fail (G_IS_ITERATOR (iter));

  if (g_atomic_int_dec_and_test (&iterator->ref_count))
    {
      G_ITERATOR_GET_CLASS (iterator)->destroy (iterator);

      g_type_free_instance ((GTypeInstance *) iterator);
    }
}

/**
 * g_iterator_next:
 * @iter: (type GIterator): FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gboolean
g_iterator_next (gpointer iter)
{
  g_return_val_if_fail (G_IS_ITERATOR (iter), FALSE);

  return G_ITERATOR_GET_CLASS (iter)->next (iter);
}

/**
 * g_value_set_iterator:
 * @value: FIXME
 * @iter: (type GIterator): FIXME
 *
 * FIXME
 */
void
g_value_set_iterator (GValue *value,
                      gpointer iter)
{
  GIterator *old_iter;

  g_return_if_fail (G_VALUE_HOLDS_ITERATOR (value));

  old_iter = value->data[0].v_pointer;

  if (iter != NULL)
    {
      g_return_if_fail (G_IS_ITERATOR (iter));

      value->data[0].v_pointer = g_iterator_ref (iter);
    }
  else
    value->data[0].v_pointer = NULL;

  if (old_iter != NULL)
    g_iterator_unref (old_iter);
}

/**
 * g_value_take_iterator:
 * @value: FIXME
 * @iter: (type GIterator) (transfer none): FIXME
 *
 * FIXME
 */
void
g_value_take_iterator (GValue *value,
                       gpointer iter)
{
  GIterator *old_iter;

  g_return_if_fail (G_VALUE_HOLDS_ITERATOR (value));

  old_iter = value->data[0].v_pointer;

  if (iter != NULL)
    {
      g_return_if_fail (G_IS_ITERATOR (iter));

      value->data[0].v_pointer = iter;
    }
  else
    value->data[0].v_pointer = NULL;

  if (old_iter != NULL)
    g_iterator_unref (old_iter);
}

/**
 * g_value_get_iterator:
 * @value: FIXME
 *
 * FIXME
 *
 * Return value: (transfer none): FIXME
 */
gpointer
g_value_get_iterator (const GValue *value)
{
  g_return_val_if_fail (G_VALUE_HOLDS_ITERATOR (value), NULL);

  return value->data[0].v_pointer;
}

/**
 * g_value_dup_iterator:
 * @value: FIXME
 *
 * FIXME
 *
 * Return value: (type GIterator) (transfer full): FIXME
 */
gpointer
g_value_dup_iterator (const GValue *value)
{
  g_return_val_if_fail (G_VALUE_HOLDS_ITERATOR (value), NULL);

  if (value->data[0].v_pointer != NULL)
    return g_iterator_ref (value->data[0].v_pointer);

  return NULL;
}
