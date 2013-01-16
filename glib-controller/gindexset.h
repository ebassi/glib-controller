#ifndef __G_INDEX_SET_H__
#define __G_INDEX_SET_H__

#include <glib-controller/gcontrollertypes.h>

G_BEGIN_DECLS

/**
 * GIndexSetPredicate:
 * @G_INDEX_SET_LESS_THAN:
 * @G_INDEX_SET_LESS_THAN_OR_EQUAL:
 * @G_INDEX_SET_GREATER_THAN_OR_EQUAL:
 * @G_INDEX_SET_GREATER_THAN:
 *
 * Predicates for g_index_set_get_index().
 */
typedef enum {
  G_INDEX_SET_LESS_THAN,
  G_INDEX_SET_LESS_THAN_OR_EQUAL,
  G_INDEX_SET_GREATER_THAN_OR_EQUAL,
  G_INDEX_SET_GREATER_THAN
} GIndexSetPredicate;

/**
 * G_INDEX_NOT_FOUND:
 */
enum {
  G_INDEX_NOT_FOUND = G_MAXINT
};

/**
 * GIndexSetEnumerateFlags:
 * @G_INDEX_SET_ENUMERATE_REVERSE:
 *
 * Flags for g_index_set_enumerate() and g_index_set_enumerate_in_range().
 */
typedef enum {
  G_INDEX_SET_ENUMERATE_REVERSE = 1 << 0
} GIndexSetEnumerateFlags;

typedef gboolean (* GIndexSetEnumerateFunc) (guint    index_,
                                             gpointer data);

GType g_index_set_get_type (void) G_GNUC_CONST;

/* Allocation */
GIndexSet *     g_index_set_alloc               (void);
GIndexSet *     g_index_set_ref                 (GIndexSet              *set);
void            g_index_set_unref               (GIndexSet              *set);

/* Initialization */
GIndexSet *     g_index_set_init                (GIndexSet              *set);
GIndexSet *     g_index_set_init_with_index     (GIndexSet              *set,
                                                 guint                   index_);
GIndexSet *     g_index_set_init_with_range     (GIndexSet              *set,
                                                 const GRange           *range);
GIndexSet *     g_index_set_init_with_set       (GIndexSet              *set,
                                                 GIndexSet              *source);

/* Query */
guint           g_index_set_count               (const GIndexSet        *set);
gboolean        g_index_set_contains_index      (const GIndexSet        *set,
                                                 guint                   index_);
gboolean        g_index_set_contains_indices    (const GIndexSet        *set,
                                                 const GIndexSet        *indices);
gboolean        g_index_set_contains_range      (const GIndexSet        *set,
                                                 const GRange           *range);

/* Index retrieval */
guint           g_index_set_first_index         (const GIndexSet        *set);
guint           g_index_set_last_index          (const GIndexSet        *set);
guint           g_index_set_get_index           (const GIndexSet        *set,
                                                 GIndexSetPredicate      predicate,
                                                 guint                   index_);
guint           g_index_set_get_indices         (const GIndexSet        *set,
                                                 guint                  *buffer,
                                                 guint                   buffer_len,
                                                 GRange                 *range);

/* Enumeration */
void            g_index_set_enumerate           (const GIndexSet        *set,
                                                 GIndexSetEnumerateFlags flags,
                                                 GIndexSetEnumerateFunc  func,
                                                 gpointer                data);
void            g_index_set_enumerate_in_range  (const GIndexSet        *set,
                                                 const GRange           *range,
                                                 GIndexSetEnumerateFlags flags,
                                                 GIndexSetEnumerateFunc  func,
                                                 gpointer                data);

G_END_DECLS

#endif /* __G_INDEX_SET_H__ */
