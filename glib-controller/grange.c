#include "grange.h"

static gpointer g_range_copy_internal (gpointer boxed);

G_DEFINE_BOXED_TYPE (GRange, g_range, g_range_copy_internal, g_range_free)

GRange *
g_range_alloc (void)
{
  return g_slice_new0 (GRange);
}

GRange *
g_range_init (GRange *range,
              guint   location,
              guint   length)
{
  if (G_LIKELY (range != NULL))
    {
      range->location = location;
      range->length = length;
    }

  return range;
}

GRange *
g_range_init_with_range (GRange       *range,
                         const GRange *src)
{
  if (G_LIKELY (range != NULL))
    *range = *src;

  return range;
}

void
g_range_free (GRange *range)
{
  if (G_LIKELY (range != NULL))
    g_slice_free (GRange, range);
}

static gpointer
g_range_copy_internal (gpointer boxed)
{
  if (G_LIKELY (boxed != NULL))
    return g_slice_dup (GRange, boxed);

  return NULL;
}
