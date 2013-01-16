#include "config.h"

#include "gindexset.h"
#include "grange.h"

struct _GIndexSet
{
  volatile gint ref_count;

  GArray *indices;
};

GIndexSet *
g_index_set_alloc (void)
{
  GIndexSet *set = g_slice_new0 (GIndexSet);

  set->ref_count = 1;

  return set;
}

GIndexSet *
g_index_set_ref (GIndexSet *set)
{
  g_return_val_if_fail (set != NULL, NULL);

  g_atomic_int_inc (&set->ref_count);

  return set;
}

void
g_index_set_unref (GIndexSet *set)
{
  g_return_if_fail (set != NULL);

  if (g_atomic_int_dec_and_test (&set->ref_count))
    {
      if (set->indices != NULL)
        g_array_unref (set->indices);

      g_slice_free (GIndexSet, set);
    }
}

GIndexSet *
g_index_set_init (GIndexSet *set)
{
  g_return_val_if_fail (set != NULL, NULL);

  if (set->indices == NULL)
    set->indices = g_array_new (FALSE, FALSE, sizeof (guint));
  else
    g_array_set_size (set->indices, 0);

  return set;
}

GIndexSet *
g_index_set_init_with_index (GIndexSet *set,
                             guint      index_)
{
  g_return_val_if_fail (set != NULL, NULL);

  if (set->indices == NULL)
    set->indices = g_array_new (FALSE, FALSE, sizeof (guint));
  else
    g_array_set_size (set->indices, 0);

  g_array_insert_val (set->indices, 0, index_);

  return set;
}

GIndexSet *
g_index_set_init_with_range (GIndexSet    *set,
                             const GRange *range)
{
  guint i;

  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (range != NULL, set);

  if (set->indices == NULL)
    set->indices = g_array_new (FALSE, FALSE, sizeof (guint));
  else
    g_array_set_size (set->indices, range->length);

  for (i = range->location; i < range->length; i++)
    g_array_insert_val (set->indices, i - range->location, i);

  return set;
}

GIndexSet *
g_index_set_init_with_set (GIndexSet *set,
                           GIndexSet *source)
{
  guint i;

  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (source != NULL, set);

  if (set->indices == NULL)
    set->indices = g_array_new (FALSE, FALSE, sizeof (guint));
  else
    g_array_set_size (set->indices, 0);

  if (source->indices == NULL)
    return set;

  for (i = 0; i < source->indices->len; i++)
    {
      guint idx = g_array_index (source->indices, guint, i);

      g_array_insert_val (set->indices, i, idx);
    }

  return set;
}

guint
g_index_set_count (const GIndexSet *set)
{
  g_return_val_if_fail (set != NULL, 0);

  if (set->indices == NULL)
    return G_INDEX_NOT_FOUND;

  return set->indices->len;
}

gboolean
g_index_set_contains_index (const GIndexSet *set,
                            guint            index_)
{
  guint i;

  g_return_val_if_fail (set != NULL, FALSE);

  if (set->indices == NULL)
    return FALSE;

  for (i = 0; i < set->indices->len; i++)
    {
      guint idx = g_array_index (set->indices, guint, i);

      if (index_ == idx)
        return TRUE;
    }

  return FALSE;
}

gboolean
g_index_set_contains_indices (const GIndexSet *set,
                              const GIndexSet *indices)
{
  return FALSE;
}

gboolean
g_index_set_contains_range (const GIndexSet *set,
                            const GRange    *range)
{
  return FALSE;
}

guint
g_index_set_first_index (const GIndexSet *set)
{
  g_return_val_if_fail (set != NULL, G_INDEX_NOT_FOUND);

  if (set->indices == NULL)
    return G_INDEX_NOT_FOUND;

  return g_array_index (set->indices, guint, 0);
}

guint
g_index_set_last_index (const GIndexSet *set)
{
  g_return_val_if_fail (set != NULL, G_INDEX_NOT_FOUND);

  if (set->indices == NULL)
    return G_INDEX_NOT_FOUND;

  return g_array_index (set->indices, guint, set->indices->len - 1);
}

guint
g_index_set_get_index (const GIndexSet    *set,
                       GIndexSetPredicate  predicate,
                       guint               index_)
{
  guint i;

  g_return_val_if_fail (set != NULL, G_INDEX_NOT_FOUND);

  if (set->indices == NULL)
    return G_INDEX_NOT_FOUND;

  for (i = 0; i < set->indices->len; i++)
    {
      guint idx = g_array_index (set->indices, guint, i);

      switch (predicate)
        {
        case G_INDEX_SET_LESS_THAN:
          if (idx < index_)
            return idx;
          break;

        case G_INDEX_SET_LESS_THAN_OR_EQUAL:
          if (idx <= index_)
            return idx;
          break;

        case G_INDEX_SET_GREATER_THAN_OR_EQUAL:
          if (idx >= index_)
            return idx;
          break;

        case G_INDEX_SET_GREATER_THAN:
          if (idx > index_)
            return idx;
          break;
        }
    }

  return G_INDEX_NOT_FOUND;
}

guint
g_index_set_get_indices (const GIndexSet *set,
                         guint           *buffer,
                         guint            buffer_len,
                         GRange          *range)
{
  g_return_val_if_fail (set != NULL, 0);
  g_return_val_if_fail (buffer != NULL && buffer_len != 0, 0);

  return 0;
}

void
g_index_set_enumerate (const GIndexSet         *set,
                       GIndexSetEnumerateFlags  flags,
                       GIndexSetEnumerateFunc   func,
                       gpointer                 data)
{
  guint start, end, step;

  g_return_if_fail (set != NULL);
  g_return_if_fail (func != NULL);

  if (flags & G_INDEX_SET_ENUMERATE_REVERSE)
    {
      start = set->indices->len - 1;
      end = 0;
      step = -1;
    }
  else
    {
      start = 0;
      end = set->indices->len - 1;
      step = 1;
    }

  for (i = start; i <= len; i += step)
    {
      guint idx = g_array_index (set->indices, guint, i);

      if (func (idx, data))
        break;
    }
}

void
g_index_set_enumerate_in_range (const GIndexSet         *set,
                                const GRange            *range,
                                GIndexSetEnumerateFlags  flags,
                                GIndexSetEnumerateFunc   func,
                                gpointer                 data)
{
}
