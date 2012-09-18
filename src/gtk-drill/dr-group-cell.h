// cell that goes into group sidebar pane
#ifndef __DR_GROUP_CELL_H__
#define __DR_GROUP_CELL_H__

#include <gtk-3.0/gtk/gtk.h>

G_BEGIN_DECLS

#define DR_GROUP_CELL_TYPE	(dr_group_cell_get_type())
#define DR_GROUP_CELL(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_GROUP_CELL_TYPE, DrGroupCell))
#define DR_GROUP_CELL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), \
			DR_GROUP_CELL_TYPE, DrGroupCellClass))
#define IS_GROUP_CELL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_GROUP_CELL_TYPE))
#define IS_GROUP_CELL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_GROUP_CELL_TYPE))

typedef struct _DrGroupCell DrGroupCell;
typedef struct _DrGroupCellClass DrGroupCellClass;
typedef struct _DrGroupCellPrivate DrGroupCellPrivate;

struct _DrGroupCell {
	GtkVBox base_instance;

	DrGroupCellPrivate *priv;
};

struct _DrGroupCellClass {
	GtkVBoxClass parent_class;
	void (* dr_groupcell) (DrGroupCell *groupcell);
};

GType dr_group_cell_get_type(void);
GtkWidget *dr_group_cell_new(void);

G_END_DECLS
#endif // __DR_GROUP_CELL_H__

struct stack_pool_proto
{
	struct stack_pool_proto *next;
	gpointer data;
};
typedef struct stack_pool_proto stack_pool_t;

int dr_group_cell_get_container_type(GtkWidget *widget);
void dr_group_cell_set_container_type(GtkWidget *widget, int type);
group_t *dr_group_cell_get_group(GtkWidget *groupcell);
void dr_group_cell_set_group(GtkWidget *groupcell, group_t *group);
form_t *dr_group_cell_get_form(GtkWidget *widget);
void dr_group_cell_set_form(GtkWidget *groupcell_widget, form_t *form);
GtkWidget *dr_group_cell_get_next(GtkWidget *groupcell);
bool dr_group_cell_has_next(GtkWidget *groupcell);
GtkWidget *dr_group_cell_add(GtkWidget *groupcell, group_t *group);
GtkWidget *dr_group_cell_append(GtkWidget *widget, group_t *group, form_t *form);
GtkWidget *dr_group_cell_delete_from(GtkWidget *widget, GtkWidget *container);
void dr_group_cell_set_is_this_set(GtkWidget *widget, bool is_this_set);
bool dr_group_cell_get_is_this_set(GtkWidget *widget);
void dr_group_cell_transplant_cell(GtkWidget *widget, gpointer *data);
bool dr_group_cell_check_form_nonlocal(GtkWidget *widget);

