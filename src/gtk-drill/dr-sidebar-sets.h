
#ifndef __SIDEBAR_SETS_H__
#define __SIDEBAR_SETS_H__

#include <gtk-3.0/gtk/gtk.h>

G_BEGIN_DECLS

#define DR_SIDEBAR_SETS_TYPE	(dr_sidebar_sets_get_type())
#define DR_SIDEBAR_SETS(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_SIDEBAR_SETS_TYPE, DrSidebarSets))
#define DR_SIDEBAR_SETS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), \
			DR_SIDEBAR_SETS_TYPE, DrSidebarSetsClass))
#define IS_SIDEBAR_SETS(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_SIDEBAR_SETS_TYPE))
#define IS_SIDEBAR_SETS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_SIDEBAR_SETS_TYPE))

typedef struct _DrSidebarSets DrSidebarSets;
typedef struct _DrSidebarSetsClass DrSidebarSetsClass;
typedef struct _DrSidebarSetsPrivate DrSidebarSetsPrivate;

struct _DrSidebarSets {
	GtkVBox base_instance;

	DrSidebarSetsPrivate *priv;
};

struct _DrSidebarSetsClass {
	GtkVBoxClass parent_class;
	void (* dr_perfbar) (DrSidebarSets *sidebar_sets);
};

GType dr_sidebar_sets_get_type(void);
GtkWidget *dr_sidebar_sets_new(void);

G_END_DECLS
#endif // __SIDEBAR_SETS_H__

void dr_sidebar_sets_update(GtkWidget *sidebarsets);


