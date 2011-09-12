#if !defined(__GLIB_CONTROLLER_H_INSIDE__) && !defined(GLIB_CONTROLLER_COMPILATION)
#error "Only <glib-controller/glib-controller.h> can be included directly."
#endif

#ifndef __G_CONTROLLER_EVENT_H__
#define __G_CONTROLLER_EVENT_H__

#include <glib-controller/gcontrollertypes.h>

G_BEGIN_DECLS

#define G_CONTROLLER_EVENT_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_CONTROLLER_EVENT, GControllerEventClass))
#define G_IS_CONTROLLER_EVENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_CONTROLLER_EVENT))
#define G_CONTROLLER_EVENT_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_CONTROLLER_EVENT, GControllerEventClass))

typedef struct _GControllerEventPrivate         GControllerEventPrivate;
typedef struct _GControllerEventClass           GControllerEventClass;

struct _GControllerEvent
{
  /*< private >*/
  GObject parent_instance;

  GControllerEventPrivate *priv;
};

/**
 * GControllerEventClass:
 *
 * The <structname>GControllerEventClass</structname> structure
 * contains only private data
 */
struct _GControllerEventClass
{
  /*< private >*/
  GObjectClass parent_class;

  /* padding */
  void (*_g_controller_event0) (void);
  void (*_g_controller_event1) (void);
  void (*_g_controller_event2) (void);
  void (*_g_controller_event3) (void);
  void (*_g_controller_event4) (void);
  void (*_g_controller_event5) (void);
  void (*_g_controller_event6) (void);
  void (*_g_controller_event7) (void);
  void (*_g_controller_event8) (void);
};

GType g_controller_event_get_type (void) G_GNUC_CONST;

GController *           g_controller_event_get_controller       (GControllerEvent *event);
GControllerAction       g_controller_event_get_action           (GControllerEvent *event);
gint                    g_controller_event_get_n_indices        (GControllerEvent *event);
GType                   g_controller_event_get_index_type       (GControllerEvent *event);

void                    g_controller_event_add_index_value      (GControllerEvent *event,
                                                                 const GValue     *value);
gboolean                g_controller_event_get_index_value      (GControllerEvent *event,
                                                                 gint              index_,
                                                                 GValue           *value);

void                    g_controller_event_add_index            (GControllerEvent *event,
                                                                 ...);
gboolean                g_controller_event_get_index            (GControllerEvent *event,
                                                                 gint              index_,
                                                                 ...);

gint                    g_controller_event_get_index_int        (GControllerEvent *event,
                                                                 gint              index_);
guint                   g_controller_event_get_index_uint       (GControllerEvent *event,
                                                                 gint              index_);
const gchar *           g_controller_event_get_index_string     (GControllerEvent *event,
                                                                 gint              index_);
gpointer                g_controller_event_get_index_pointer    (GControllerEvent *event,
                                                                 gint              index_);

G_END_DECLS

#endif /* __G_CONTROLLER_EVENT_H__ */
