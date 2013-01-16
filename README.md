GController :: A Controller implementation for GLib data types
===============================================================================

The story so far...
-------------------------------------------------------------------------------
Given a model-view-controller design, the current approach in GLib-based
libraries is to encapsulate the data structure used for storage and add
storage accessors that notify the view(s) of changes - thus conflating the
model and the controller sides into one data structure.

Encapsulation of storage and controller has invariably led to some issues:

 * data replication: unless the model is an interface that resides at a
   lower level in the dependency chain, and that can be bolted on top of
   an existing data storage, there has to be a copy from the simple data
   storage types provided by GLib (GArray, GList, GHashTable, etc.) into
   the model itself.

 * data access replication: be the model an interface or a base class,
   it requires the implementation of an API to access the data from the
   storage, including iteration over the data storage.

 * loss of performance/generality: if a model has to be accessible from
   higher levels of the platform stack, it has to lose every knowledge
   of data types, or re-implement them abstractly, incurring in a loss
   of performance; conversely, if it can be placed at higher levels of
   the stack, it will become tied to the view structure, losing
   generality.

These issues have been nominally approached by different libraries in
different ways - but always trying to maintain the model (data storage)
and the controller angles together.

GController: An alternative angle of attack
-------------------------------------------------------------------------------
Instead of collapsing both model and controller inside the same class it
should be possible to devise a way to use simple data structures already
provided by GLib for storage, access and iteration, and decouple the
controller into a separate class.

### Design requirements ###

 * must work with GLib data types:
   + GArray/GPtrArray
   + GSequence
   + GList/GSList
   + GHashTable
 * must not require changes to those data types
   + must be able to notify actions on the data types
 * must defer data access and iteration over to the data types

### GController ###

GController is the base, abstract class for controllers.

A specialized implementation of GController for a data type might be
provided - e.g. GArrayController for GArray - but it is not necessary; a
specialized implementation of GController for a model class might also
be provided, though it's not necessary.

Every time an action is performed on the GArray, the GController has to:

  * create a GControllerReference
  * emit the GController::changed signal

For instance, given a GController created for a given GArray:

    GController *controller = g_array_controller_new (array);

The GArrayController code for appending a value inside the GArray would
look something like:

    g_array_append (array, value);

    GIndexSet *index =
      g_index_set_init_with_index (g_index_set_alloc (), array->len - 1);

    GControllerEvent *event =
      g_controller_create_event (controller, G_CONTROLLER_ADD, index);

    g_controller_emit_changed (controller, ref);
    g_object_unref (ref);

This will result in the _GController::changed_ signal being emitted; the
signal has the following prototype:

    void (* changed) (GController       *controller,
                      GControllerAction  action,
                      GControllerEvent  *event);

Implementations of the view for a given controller should use the changed
signal to update themselves, for instance:

    static void
    on_changed (GController       *controller,
                GControllerAction  action,
                GControllerEvent  *event)
    {
      GArray *array =
        g_array_controller_get_array (G_ARRAY_CONTROLLER (controller));
      const GIndexSet *indices =
        g_controller_event_get_indices (event);

      guint i = 0;
      while (g_index_set_next_index (indices, &i))
        {
          /* get the value out of the array at the given index */
          Data *data = &g_array_index (array, Data, i);

          /* do something with Data */
        }
    }

### GControllerEvent ###

A GControllerEvent is an object created by each GController, and it
references four things:

 * the action that led to the creation of the reference
 * the type of the index inside the data storage controlled by
   the GController
 * the number of indices affected by the action on the data storage
   that caused the emission of the changed signal
 * the indices affected by the action on the data storage

The GControllerEvent does not store or replicate the data affected by
the action recorded by the GController: it merely references where the
View might find the data inside the storage.

### Indices and Ranges ###

FIXME

