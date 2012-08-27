
#ifndef __SIDEBAR_GROUPS_H__
#define __SIDEBAR_GROUPS_H__

#include <gtk/gtk.h>

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

struct group_box_proto
{
	// used on group sidebar tab
	// to show group information
	group_t *group;
	GtkWidget *button;

	struct group_box_proto *next;
};

typedef struct group_box_proto group_box_t;


void dr_sidebar_groups_update(GtkWidget *sidebargroups);
group_box_t *dr_create_group_box(void);
group_box_t *dr_add_group_box_from_select(group_box_t *gbox, select_t *select);



