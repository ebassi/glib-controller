#ifndef __G_RANGE_H__
#define __G_RANGE_H__

#include <glib-controller/gcontrollertypes.h>

G_BEGIN_DECLS

struct _GRange
{
  guint location;
  guint length;
};

#define G_RANGE_INIT(loc,len)   { (loc), (len) }
#define G_RANGE_INIT_ZERO       G_RANGE_INIT (0, 0)

GType g_range_get_type (void) G_GNUC_CONST;

GRange *        g_range_alloc           (void);
GRange *        g_range_init            (GRange       *range,
                                         guint         location,
                                         guint         length);
GRange *        g_range_init_with_range (GRange       *range,
                                         const GRange *src);
void            g_range_free            (GRange       *range);

G_END_DECLS

#endif /* __G_RANGE_H__ */
