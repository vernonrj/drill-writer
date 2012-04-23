
#ifndef __SIDEBAR_H__
#define __SIDEBAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DR_SIDEBAR_TYPE	(dr_sidebar_get_type())
#define DR_SIDEBAR(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_SIDEBAR_TYPE, DrSidebar))
#define DR_SIDEBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), DR_SIDEBAR_TYPE, DrSidebarClass))
#define IS_SIDEBAR(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_SIDEBAR_TYPE))
#define IS_SIDEBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_SIDEBAR_TYPE))

typedef struct _DrSidebar DrSidebar;
typedef struct _DrSidebarClass DrSidebarClass;
typedef struct _DrSidebarPrivate DrSidebarPrivate;

struct _DrSidebar {
	GtkVBox base_instance;
	DrSidebarPrivate *priv;
};

struct _DrSidebarClass {
	GtkVBoxClass parent_class;
	void (* dr_sidebar) (DrSidebar *sidebar);
};

GType dr_sidebar_get_type(void);
GtkWidget *dr_sidebar_new(void);

G_END_DECLS
#endif // __SIDEBAR_H__


GtkWidget *create_combo_box (const char **strings);
void dr_sidebar_changed(GtkComboBox *widget, DrSidebar *lsidebar);
void dr_sidebar_update(DrSidebar *lsidebar);
void dr_sidebar_toggle(GtkWidget *widget);

