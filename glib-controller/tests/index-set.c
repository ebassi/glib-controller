#include <glib-controller/glib-controller.h>

static void
init_empty (void)
{
  GIndexSet *set;

  set = g_index_set_init (g_index_set_alloc ());
  g_assert_cmpint (g_index_set_count (set), ==, 0);

  g_index_set_unref (set);
}

static void
init_index (void)
{
  GIndexSet *set;

  set = g_index_set_init_with_index (g_index_set_alloc (), 42);
  g_assert_cmpint (g_index_set_count (set), ==, 1);
  g_assert_cmpint (g_index_set_first_index (set), ==, 42);
  g_assert_cmpint (g_index_set_last_index (set), ==, 42);

  g_index_set_unref (set);
}

static void
init_range (void)
{
  GRange range = G_RANGE_INIT (0, 100);
  GIndexSet *set;

  set = g_index_set_init_with_range (g_index_set_alloc (), &range);
  g_assert_cmpint (g_index_set_count (set), ==, 100);
  g_assert_cmpint (g_index_set_first_index (set), ==, 0);
  g_assert_cmpint (g_index_set_last_index (set), ==, 99);

  g_assert (g_index_set_contains_index (set, g_random_int_range (0, 100)));

  g_index_set_unref (set);
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/index-set/init-empty", init_empty);
  g_test_add_func ("/index-set/init-index", init_index);
  g_test_add_func ("/index-set/init-range", init_range);

  return g_test_run ();
}
