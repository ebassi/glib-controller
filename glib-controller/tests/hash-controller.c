#include <glib-controller/glib-controller.h>
#include <string.h>
#include <stdlib.h>

static void
hash_constructor (void)
{
  GController *controller;
  GHashTable *hash;

  hash = g_hash_table_new (NULL, NULL);
  controller = g_hash_controller_new_with_hash (hash);
  g_assert (G_IS_HASH_CONTROLLER (controller));
  g_assert (g_hash_controller_get_hash (G_HASH_CONTROLLER (controller)) == hash);

  g_object_unref (controller);
  g_hash_table_destroy (hash);
}

static void
hash_create_event (void)
{
  GController *controller;
  GControllerEvent *event;

  controller = g_hash_controller_new ();
  event = g_controller_create_event (controller, G_CONTROLLER_CLEAR, G_TYPE_POINTER, 1, GINT_TO_POINTER (0xdeadbeef));

  g_assert (G_IS_CONTROLLER_EVENT (event));
  g_assert (g_controller_event_get_index_type (event) == G_TYPE_POINTER);
  g_assert_cmpint (g_controller_event_get_n_indices (event), ==, 1);
  g_assert (g_controller_event_get_index_pointer (event, 0) == GINT_TO_POINTER (0xdeadbeef));

  g_object_unref (event);
  g_object_unref (controller);
}

typedef struct _ChangedClosure {
  GControllerAction action;
  GType index_type;
  gint n_indices;
  gchar **indices;
} ChangedClosure;

static void
on_changed (GController *controller,
            GControllerAction action,
            GControllerEvent *event,
            gpointer user_data)
{
  ChangedClosure *clos = user_data;
  gint i, n_indices;
  GType index_type;

  index_type = g_controller_event_get_index_type (event);
  n_indices = g_controller_event_get_n_indices (event);

  g_assert_cmpint (action, ==, clos->action);
  g_assert (index_type == clos->index_type);
  g_assert_cmpint (n_indices, ==, clos->n_indices);

  for (i = 0; i < n_indices; i++)
    {
      const gchar *idx = g_controller_event_get_index_string (event, i);

      if (g_test_verbose ())
        g_print ("Got '%s' (%p), expected '%s' (%p)\n",
                 (char *) idx, idx,
                 (char *) clos->indices[i], clos->indices[i]);

      g_assert_cmpstr (idx, ==, clos->indices[i]);
    }
}

static void
hash_emit_changed (void)
{
  GHashTable *hash = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
  GController *controller = g_hash_controller_new_with_hash (hash);
  ChangedClosure expected = { 0, };
  GControllerEvent *ref;
  gchar *foo;
  gulong id;

  id = g_signal_connect (controller, "changed", G_CALLBACK (on_changed), &expected);

  foo = g_strdup ("foo");

  expected.action = G_CONTROLLER_ADD;
  expected.index_type = G_TYPE_STRING;
  expected.n_indices = 1;
  expected.indices = g_new (gchar*, expected.n_indices);
  expected.indices[0] = foo;

  g_hash_table_insert (hash, foo, g_strdup ("bar"));

  ref = g_controller_create_event (controller, G_CONTROLLER_ADD, G_TYPE_STRING, 1, foo);
  g_assert (G_IS_CONTROLLER_EVENT (ref));
  g_controller_emit_changed (controller, ref);

  g_object_unref (ref);
  g_free (expected.indices);

  g_signal_handler_disconnect (controller, id);

  g_object_unref (controller);
  g_hash_table_destroy (hash);
  g_free (foo);
}

int
main (int argc, char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/hash/constructor", hash_constructor);
  g_test_add_func ("/hash/create-event", hash_create_event);
  g_test_add_func ("/hash/emit-changed", hash_emit_changed);

  return g_test_run ();
}
