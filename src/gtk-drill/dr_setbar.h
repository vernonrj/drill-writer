
#ifndef __SETBAR_H__
#define __SETBAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DR_SETBAR_TYPE	(dr_setbar_get_type())
#define DR_SETBAR(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_SETBAR_TYPE, DrSetbar))
#define DR_SETBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), DR_SETBAR_TYPE, DrSetbarClass))
#define IS_SETBAR(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_SETBAR_TYPE))
#define IS_SETBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_SETBAR_TYPE))

typedef struct _DrSetbar DrSetbar;
typedef struct _DrSetbarClass DrSetbarClass;
typedef struct _DrSetbarPrivate DrSetbarPrivate;

struct _DrSetbar {
	//GtkVBox base_instance;
	GtkHBox base_instance;

	DrSetbarPrivate *priv;
};

struct _DrSetbarClass {
	//GtkVBoxClass parent_class;
	GtkHBoxClass parent_class;
	void (* dr_setbar) (DrSetbar *setbar);
};

GType dr_setbar_get_type(void);
GtkWidget *dr_setbar_new(void);

G_END_DECLS
#endif // __SETBAR_H__


// set-controls-gtk.c
void add_set_gtk(GtkWidget *widget);
void delete_set_gtk(GtkWidget *widget);
void set_first_gtk(GtkWidget *widget);
void set_last_gtk(GtkWidget *widget);
void set_next_gtk(GtkWidget *widget);
void set_next_count_gtk(GtkWidget *widget);
void set_prev_count_gtk(GtkWidget *widget);
void set_prev_gtk(GtkWidget *widget);
void set_set_name_gtk(GtkWidget *widget, DrSetbar *drsetbar);
void goto_set_gtk (GtkWidget *widget, DrSetbar *drsetbar);
void goto_count (GtkWidget *widget, DrSetbar *drsetbar);
void change_counts (GtkWidget *widget, DrSetbar *drsetbar);
void change_tempo_gtk (GtkWidget *widget, DrSetbar *drsetbar);
void dr_setbar_update_entries(GtkWidget *drsetbar);

