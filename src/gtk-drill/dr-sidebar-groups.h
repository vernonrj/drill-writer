
#ifndef __SIDEBAR_GROUPS_H__
#define __SIDEBAR_GROUPS_H__

#include <gtk-3.0/gtk/gtk.h>

G_BEGIN_DECLS

#define DR_SIDEBAR_GROUPS_TYPE	(dr_sidebar_groups_get_type())
#define DR_SIDEBAR_GROUPS(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_SIDEBAR_GROUPS_TYPE, DrSidebarGroups))
#define DR_SIDEBAR_GROUPS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), \
			DR_SIDEBAR_GROUPS_TYPE, DrSidebarGroupsClass))
#define IS_SIDEBAR_GROUPS(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_SIDEBAR_GROUPS_TYPE))
#define IS_SIDEBAR_GROUPS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_SIDEBAR_GROUPS_TYPE))

typedef struct _DrSidebarGroups DrSidebarGroups;
typedef struct _DrSidebarGroupsClass DrSidebarGroupsClass;
typedef struct _DrSidebarGroupsPrivate DrSidebarGroupsPrivate;

struct _DrSidebarGroups {
	GtkVBox base_instance;

	DrSidebarGroupsPrivate *priv;
};

struct _DrSidebarGroupsClass {
	GtkVBoxClass parent_class;
	void (* dr_perfbar) (DrSidebarGroups *sidebar_groups);
};

GType dr_sidebar_groups_get_type(void);
GtkWidget *dr_sidebar_groups_new(void);

G_END_DECLS
#endif // __SIDEBAR_GROUPS_H__

GTrashStack *stack_pool;

typedef struct group_cell_container groupcell_c;
//typedef struct groupcell_list_proto groupcell_l;
typedef struct group_cell_headlist groupcell_l;
struct group_cell_container
{
	//LIST_ENTRY(group_cell_container) groupcell_entries;
	GtkWidget *cell;
	groupcell_c *next;
	groupcell_c *prev;
};

struct group_cell_headlist
{
	groupcell_c *list_first;
};
//LIST_HEAD(groupcell_list_proto, group_cell_container);


void dr_sidebar_groups_flush_local(GtkWidget *container, groupcell_l *cell_head);
void dr_sidebar_groups_update(GtkWidget *sidebargroups);

void groupcell_list_init(groupcell_l *head);
int groupcell_list_empty(groupcell_l *head);
groupcell_c *groupcell_list_first(groupcell_l *head);
void groupcell_list_remove(groupcell_l *head, groupcell_c *last);
void groupcell_list_insert_after(groupcell_l *head, groupcell_c *curr, groupcell_c *last);
void groupcell_list_insert_before(groupcell_l *head, groupcell_c *curr, groupcell_c *last);
void groupcell_list_insert_head(groupcell_l *head, groupcell_c *curr);
void groupcell_list_append(groupcell_l *head, groupcell_c *curr);
groupcell_c *groupcell_list_next(groupcell_c *curr);
groupcell_c *groupcell_list_prev(groupcell_c *curr);


