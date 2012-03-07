#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib-object.h>
#include <glib-controller/glib-controller.h>

G_BEGIN_DECLS

#define MY_TYPE_SIMPLE_MODEL            (my_simple_model_get_type ())
#define MY_SIMPLE_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_SIMPLE_MODEL, MySimpleModel))
#define MY_IS_SIMPLE_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_SIMPLE_MODEL))

typedef struct _MySimpleModel           MySimpleModel;
typedef struct _MySimpleModelClass      MySimpleModelClass;

struct _MySimpleModel
{
  GObject parent_instance;

  GController *controller;
  GPtrArray *array;
};

struct _MySimpleModelClass
{
  GObjectClass parent_class;
};

GType my_simple_model_get_type (void) G_GNUC_CONST;

G_END_DECLS

/* implementation */

typedef struct _MySimpleIter {
  GIterator parent_instance;

  int size;
  int last_pos;
} MySimpleIter;

typedef struct _GIteratorClass  MySimpleIterClass;

GType my_simple_iter_get_type (void) G_GNUC_CONST;

static void g_iterable_iface_init (GIterableInterface *iface);

G_DEFINE_TYPE (MySimpleIter, my_simple_iter, G_TYPE_ITERATOR)

G_DEFINE_TYPE_WITH_CODE (MySimpleModel, my_simple_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_ITERABLE,
                                                g_iterable_iface_init))

static void
my_simple_model_dispose (GObject *gobject)
{
  MySimpleModel *model = MY_SIMPLE_MODEL (gobject);

  if (model->controller != NULL)
    {
      g_object_unref (model->controller);
      model->controller = NULL;
    }

  G_OBJECT_CLASS (my_simple_model_parent_class)->dispose (gobject);
}

static void
my_simple_model_class_init (MySimpleModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = my_simple_model_dispose;
}

static void
my_simple_model_init (MySimpleModel *model)
{
  GPtrArray *array = g_ptr_array_new_with_free_func (g_free);

  model->controller = g_ptr_array_controller_new_with_array (array);
  g_ptr_array_unref (array);

  model->array = array;
}

static GIterator *
my_simple_model_create_iterator (GIterable *iterable)
{
  MySimpleModel *self = MY_SIMPLE_MODEL (iterable);
  MySimpleIter *iter = g_iterator_create (my_simple_iter_get_type ());

  iter->size = self->array->len;
  iter->last_pos = -1;

  return G_ITERATOR (iter);
}

static void
g_iterable_iface_init (GIterableInterface *iface)
{
  iface->create_iterator = my_simple_model_create_iterator;
}

static gboolean
my_simple_iter_next (GIterator *iter)
{
  MySimpleIter *self = (MySimpleIter *) iter;

  if (self->last_pos + 1 < self->size)
    {
      self->last_pos += 1;

      return TRUE;
    }

  return FALSE;
}

static void
my_simple_iter_class_init (MySimpleIterClass *klass)
{
  G_ITERATOR_CLASS (klass)->next = my_simple_iter_next;
}

static void
my_simple_iter_init (MySimpleIter *self)
{
  self->last_pos = -1;
}

MySimpleModel *
my_simple_model_new (void)
{
  return g_object_new (MY_TYPE_SIMPLE_MODEL, NULL);
}

void
my_simple_model_add_text (MySimpleModel *model,
                          const gchar   *text)
{
  GControllerEvent *ref;

  g_return_if_fail (MY_IS_SIMPLE_MODEL (model));
  g_return_if_fail (text != NULL && text[0] != '\0');

  g_ptr_array_add (model->array, g_strdup (text));

  ref = g_controller_create_event (model->controller, G_CONTROLLER_ADD,
                                       G_TYPE_UINT, 1,
                                       model->array->len - 1);
  g_controller_emit_changed (model->controller, ref);
  g_object_unref (ref);
}

void
my_simple_model_remove_text (MySimpleModel *model,
                             const gchar   *text)
{
  GControllerEvent *ref;
  gint i, pos;

  g_return_if_fail (MY_IS_SIMPLE_MODEL (model));
  g_return_if_fail (text != NULL && text[0] != '\0');

  pos = -1;
  for (i = 0; i < model->array->len; i++)
    {
      if (strcmp (g_ptr_array_index (model->array, i), text) == 0)
        {
          pos = i;
          break;
        }
    }

  if (pos == -1)
    return;

  ref = g_controller_create_event (model->controller, G_CONTROLLER_REMOVE,
                                       G_TYPE_UINT, 1,
                                       pos);
  g_controller_emit_changed (model->controller, ref);
  g_object_unref (model->controller);

  g_ptr_array_remove_index (model->array, pos);
}

const gchar *
my_simple_model_get_text (MySimpleModel *model,
                          gint           pos)
{
  g_return_val_if_fail (MY_IS_SIMPLE_MODEL (model), NULL);

  return g_ptr_array_index (model->array, pos);
}

const gchar **
my_simple_model_get_items (MySimpleModel *model)
{
  g_return_val_if_fail (MY_IS_SIMPLE_MODEL (model), NULL);

  return (const gchar **) model->array->pdata;
}

void
my_simple_model_set_items (MySimpleModel       *model,
                           gint                 n_strings,
                           const gchar * const  text[])
{
  GControllerEvent *ref;
  GPtrArray *array;
  gint i;

  g_return_if_fail (MY_IS_SIMPLE_MODEL (model));

  array = g_ptr_array_sized_new (n_strings);
  for (i = 0; n_strings; i++)
    g_ptr_array_add (array, g_strdup (text[i]));

  g_ptr_array_controller_set_array (G_PTR_ARRAY_CONTROLLER (model->controller), array);

  ref = g_controller_create_event (model->controller, G_CONTROLLER_REPLACE,
                                       G_TYPE_UINT, 0);
  g_controller_emit_changed (model->controller, ref);
  g_object_unref (ref);
}

void
my_simple_model_clear (MySimpleModel *model)
{
  GControllerEvent *ref;
  GPtrArray *array;

  g_return_if_fail (MY_IS_SIMPLE_MODEL (model));

  array = g_ptr_array_new ();
  g_ptr_array_controller_set_array (G_PTR_ARRAY_CONTROLLER (model->controller), array);
  model->array = array;

  ref = g_controller_create_event (model->controller, G_CONTROLLER_CLEAR,
                                       G_TYPE_UINT, 0);
  g_controller_emit_changed (model->controller, ref);
  g_object_unref (ref);
}

gint
my_simple_model_get_size (MySimpleModel *model)
{
  g_return_val_if_fail (MY_IS_SIMPLE_MODEL (model), 0);

  return model->array->len;
}

GController *
my_simple_model_get_controller (MySimpleModel *model)
{
  g_return_val_if_fail (MY_IS_SIMPLE_MODEL (model), NULL);

  return model->controller;
}

/* user */

static void
on_model_changed (GController *controller,
                  GControllerAction action,
                  GControllerEvent *ref,
                  MySimpleModel *model)
{
  gint i, n_indices;

  n_indices = g_controller_event_get_n_indices (ref);

  switch (action)
    {
    case G_CONTROLLER_ADD:
      for (i = 0; i < n_indices; i++)
        {
          gint idx = g_controller_event_get_index_uint (ref, i);
          const gchar *text;

          text = my_simple_model_get_text (model, idx);
          g_assert (text != NULL);

          g_print ("*** Added '%s' (at index '%d') ***\n", text, idx);
        }
      break;

    case G_CONTROLLER_REMOVE:
      for (i = 0; i < n_indices; i++)
        {
          gint idx = g_controller_event_get_index_uint (ref, i);
          const gchar *text;

          text = my_simple_model_get_text (model, idx);
          g_assert (text != NULL);

          g_print ("*** Removed '%s' (at index '%d') ***\n", text, idx);
        }
      break;

    case G_CONTROLLER_CLEAR:
      g_print ("*** Model cleared ***\n");
      break;

    default:
      break;
    }
}

int
main (int argc, char *argv[])
{
  MySimpleModel *model;
  GIterator *iter;
  gint i;

  g_type_init ();

  model = my_simple_model_new ();
  g_signal_connect (my_simple_model_get_controller (model),
                    "changed",
                    G_CALLBACK (on_model_changed),
                    model);

  my_simple_model_add_text (model, "foo");
  my_simple_model_add_text (model, "bar");
  my_simple_model_add_text (model, "baz");

  i = 0;
  g_print ("Model contents:\n");
  iter = g_iterable_create_iterator (G_ITERABLE (model));
  while (g_iterator_next (iter))
    {
      g_print ("\trow[%d] = '%s'\n", i, my_simple_model_get_text (model, i));
      i += 1;
    }
  g_iterator_unref (iter);

  my_simple_model_remove_text (model, "baz");

  i = 0;
  g_print ("Model contents:\n");
  iter = g_iterable_create_iterator (G_ITERABLE (model));
  while (g_iterator_next (iter))
    {
      g_print ("\trow[%d] = '%s'\n", i, my_simple_model_get_text (model, i));
      i += 1;
    }
  g_iterator_unref (iter);

  my_simple_model_clear (model);

  g_print ("Model contents:\n");
  iter = g_iterable_create_iterator (G_ITERABLE (model));
  while (g_iterator_next (iter))
    g_assert_not_reached ();
  g_iterator_unref (iter);

  g_object_unref (model);

  return EXIT_SUCCESS;
}
