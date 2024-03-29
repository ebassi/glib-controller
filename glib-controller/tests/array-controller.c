#include <glib-controller/glib-controller.h>
#include <string.h>
#include <stdlib.h>

static void
array_constructor (void)
{
  GController *controller;
  GArray *array;

  array = g_array_new (FALSE, FALSE, sizeof (int));
  controller = g_array_controller_new_with_array (array);
  g_assert (G_IS_ARRAY_CONTROLLER (controller));
  g_assert (g_array_controller_get_array (G_ARRAY_CONTROLLER (controller)) == array);

  g_object_unref (controller);
  g_array_free (array, TRUE);
}

static void
array_create_event (void)
{
  GController *controller;
  GControllerEvent *event;

  controller = g_array_controller_new ();
  event = g_controller_create_event (controller, G_CONTROLLER_CLEAR,
                                     G_TYPE_UINT, 1,
                                     0);

  g_assert (G_IS_CONTROLLER_EVENT (event));
  g_assert (g_controller_event_get_index_type (event) == G_TYPE_UINT);
  g_assert_cmpint (g_controller_event_get_n_indices (event), ==, 1);
  g_assert_cmpint (g_controller_event_get_index_uint (event, 0), ==, 0);

  g_object_unref (event);
  g_object_unref (controller);
}

typedef struct _ChangedClosure {
  GControllerAction action;
  GType index_type;
  gint n_indices;
  guint *indices;
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
      guint idx = g_controller_event_get_index_uint (event, i);
      g_assert_cmpint (idx, ==, clos->indices[i]);
    }
}

static void
array_emit_changed (void)
{
  GArray *array = g_array_new (FALSE, FALSE, sizeof (int));
  GController *controller = g_array_controller_new_with_array (array);
  ChangedClosure expected = { 0, 0 };
  GControllerEvent *ref;
  gulong id;
  int value;

  id = g_signal_connect (controller, "changed", G_CALLBACK (on_changed), &expected);

  expected.action = G_CONTROLLER_ADD;
  expected.index_type = G_TYPE_UINT;
  expected.n_indices = 1;
  expected.indices = g_new (guint, expected.n_indices);
  expected.indices[0] = 0;

  value = 42;
  g_array_append_val (array, value);

  ref = g_controller_create_event (controller, G_CONTROLLER_ADD, G_TYPE_UINT, 1, 0);
  g_assert (G_IS_CONTROLLER_EVENT (ref));
  g_controller_emit_changed (controller, ref);

  g_object_unref (ref);
  g_free (expected.indices);

  g_signal_handler_disconnect (controller, id);

  g_object_unref (controller);
  g_array_free (array, TRUE);
}

static void
array_bulk_emit_changed (void)
{
  GArray *array = g_array_new (FALSE, FALSE, sizeof (int));
  GController *controller = g_array_controller_new_with_array (array);
  ChangedClosure expected = { 0, 0 };
  GControllerEvent *ref;
  gulong id;
  int i;

  id = g_signal_connect (controller, "changed", G_CALLBACK (on_changed), &expected);

  ref = g_controller_create_event (controller, G_CONTROLLER_ADD, G_TYPE_UINT, 0);
  g_assert (G_IS_CONTROLLER_EVENT (ref));

  expected.action = G_CONTROLLER_ADD;
  expected.index_type = G_TYPE_UINT;
  expected.n_indices = 10;
  expected.indices = g_new (guint, expected.n_indices);

  for (i = 0; i < 10; i++)
    {
      int val = i * 2;

      g_array_append_val (array, val);

      expected.indices[i] = i;
      g_controller_event_add_index (ref, i);
    }

  g_controller_emit_changed (controller, ref);

  g_object_unref (ref);
  g_free (expected.indices);

  g_signal_handler_disconnect (controller, id);

  g_object_unref (controller);
  g_array_free (array, TRUE);
}

int
main (int argc, char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/array/constructor", array_constructor);
  g_test_add_func ("/array/create-event", array_create_event);
  g_test_add_func ("/array/emit-changed", array_emit_changed);
  g_test_add_func ("/array/emit-bulk-changed", array_bulk_emit_changed);

  return g_test_run ();
}
