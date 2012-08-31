
#ifndef __SIDEBAR_PERFS_H__
#define __SIDEBAR_PERFS_H__

#include <gtk-3.0/gtk/gtk.h>

G_BEGIN_DECLS

#define DR_SIDEBAR_PERFS_TYPE	(dr_sidebar_perfs_get_type())
#define DR_SIDEBAR_PERFS(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_SIDEBAR_PERFS_TYPE, DrSidebarPerfs))
#define DR_SIDEBAR_PERFS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), \
			DR_SIDEBAR_PERFS_TYPE, DrSidebarPerfsClass))
#define IS_SIDEBAR_PERFS(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_SIDEBAR_PERFS_TYPE))
#define IS_SIDEBAR_PERFS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_SIDEBAR_PERFS_TYPE))

typedef struct _DrSidebarPerfs DrSidebarPerfs;
typedef struct _DrSidebarPerfsClass DrSidebarPerfsClass;
typedef struct _DrSidebarPerfsPrivate DrSidebarPerfsPrivate;

struct _DrSidebarPerfs {
	GtkVBox base_instance;

	DrSidebarPerfsPrivate *priv;
};

struct _DrSidebarPerfsClass {
	GtkVBoxClass parent_class;
	void (* dr_perfbar) (DrSidebarPerfs *sidebar_perfs);
};

GType dr_sidebar_perfs_get_type(void);
GtkWidget *dr_sidebar_perfs_new(void);

G_END_DECLS
#endif // __SIDEBAR_PERFS_H__

void dr_sidebar_perfs_update(GtkWidget *sidebarperfs);



