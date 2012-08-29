#include "d_gtk.h"
//#include "dr-sidebar-groups.h"

static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	//GtkWidget *entry_groupname;
	GtkWidget *group_cell;
	GtkWidget *group_cell_local;
	GtkWidget *global_frame, *local_frame;
	GtkWidget *box_global, *box_local;
	set_t *currset;
	//group_box_t *group_box;
};

G_DEFINE_TYPE (DrSidebarGroups, dr_sidebar_groups, GTK_TYPE_VBOX)

#define DR_SIDEBAR_GROUPS_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SIDEBAR_GROUPS_TYPE, DrSidebarGroupsPrivate))

static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSidebarGroupsPrivate));
}

static void dr_sidebar_groups_init(DrSidebarGroups *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	GtkWidget *button_add_group;


	sidebargroups->priv = DR_SIDEBAR_GROUPS_GET_PRIVATE(sidebargroups);
	sidebargroups->priv->group_cell = NULL;
	sidebargroups->priv->group_cell_local = NULL;
	sidebargroups->priv->currset = pshow->sets->currset;

	button_add_group = gtk_button_new_with_label("Add group");
	gtk_widget_show(button_add_group);
	gtk_box_pack_start(GTK_BOX(sidebargroups), button_add_group, FALSE, FALSE, 5);
	g_signal_connect(button_add_group, "clicked", G_CALLBACK(add_group_gtk), NULL);


	sidebargroups->priv->global_frame = gtk_frame_new("Global groups");
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->global_frame, FALSE, FALSE, 0);
	sidebargroups->priv->box_global = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->global_frame), sidebargroups->priv->box_global);

	sidebargroups->priv->local_frame = gtk_frame_new("Local groups");
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->local_frame, FALSE, FALSE, 0);
	sidebargroups->priv->box_local = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->local_frame), sidebargroups->priv->box_local);


	return;
}

GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}

DrSidebarGroups *dr_sidebar_groups_update_from(DrSidebarGroups *lsidebargroups, GtkWidget **last_r, group_t **group_r)
{
	GtkWidget *last;
	group_t *group;
	last = *last_r;
	group = *group_r;
	GtkWidget *lastcurr = last;
	bool listEmpty = false;
	while (last && group)
	{
		if (!dr_group_cell_get_group(last))
		{
			gtk_widget_hide(last);
			last = dr_group_cell_delete_from(last, lastcurr);
		}
		else
		{
			lastcurr = last;
			last = dr_group_cell_get_next(lastcurr);
			//lastgroup = group;
			group = group->next;
		}
	}
	while (last)
	{
		if (!dr_group_cell_get_group(last))
		{
			gtk_widget_hide(last);
			if (last == lastcurr)
				listEmpty = true;
			last = dr_group_cell_delete_from(last, lastcurr);
		}
		else
		{
			listEmpty = false;
			lastcurr = last;
			last = dr_group_cell_get_next(last);
		}
	}
	if (!listEmpty)
		last = lastcurr;
	else
		last = NULL;
	*last_r = last;
	*group_r = group;
	return lsidebargroups;
}


void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	DrSidebarGroups *lsidebargroups;

	lsidebargroups = (DrSidebarGroups*)sidebargroups;
	GtkWidget *last = lsidebargroups->priv->group_cell;
	GtkWidget *lastcurr = last;
	group_t *group = pshow->topgroups;


	lsidebargroups = dr_sidebar_groups_update_from(lsidebargroups, &last, &group);
	if (!last)
		lsidebargroups->priv->group_cell = NULL;
	while (group)
	{
		// added a new group
		// add another sidebar ref
		if (!last)
		{
			last = dr_group_cell_new();
			lsidebargroups->priv->group_cell = last;
			dr_group_cell_set_group(last, group);
		}
		else
		{
			last = dr_group_cell_append(last, group);
			last = dr_group_cell_get_next(last);
		}
		gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_global), last, FALSE, FALSE, 0);
		gtk_widget_show(last);
		group = group->next;
	}
	last = lsidebargroups->priv->group_cell_local;
	group = pshow->sets->currset->groups;
	if (pshow->sets->currset != lsidebargroups->priv->currset)
	{
		// flush the local groups out
		lastcurr = last;
		while (last)
		{
			gtk_widget_hide(last);
			last = dr_group_cell_delete_from(last, lastcurr);
		}
		lsidebargroups->priv->group_cell_local = NULL;
		last = NULL;
		lsidebargroups->priv->currset = pshow->sets->currset;
	}
	lsidebargroups = dr_sidebar_groups_update_from(lsidebargroups, &last, &group);
	if (!last)
		lsidebargroups->priv->group_cell_local = NULL;
	while (group)
	{
		// added a new group
		// add another sidebar ref
		if (!last)
		{
			last = dr_group_cell_new();
			lsidebargroups->priv->group_cell_local = last;
			dr_group_cell_set_group(last, group);
		}
		else
		{
			last = dr_group_cell_append(last, group);
			last = dr_group_cell_get_next(last);
		}
		gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_local), last, FALSE, FALSE, 0);
		gtk_widget_show(last);
		group = group->next;
	}


	if (!lsidebargroups->priv->group_cell)
	{
		gtk_widget_hide(lsidebargroups->priv->box_global);
		gtk_widget_hide(lsidebargroups->priv->global_frame);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_global);
		gtk_widget_show(lsidebargroups->priv->global_frame);
	}
	if (!lsidebargroups->priv->group_cell_local)
	{
		gtk_widget_hide(lsidebargroups->priv->local_frame);
		gtk_widget_hide(lsidebargroups->priv->box_local);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_local);
		gtk_widget_show(lsidebargroups->priv->local_frame);
	}
	return;
}


group_box_t *dr_create_group_box(void)
{
	group_box_t *gbox;
	gbox = (group_box_t*)malloc(sizeof(group_box_t));
	gbox->group = NULL;
	gbox->next = NULL;
	return gbox;
}

group_box_t *dr_add_group_box_from_select(group_box_t *gbox, select_t *select)
{
	group_box_t *last;
	group_box_t *curr;

	last = dr_create_group_box();
	last->group = group_add_selects(last->group, select);
	if (!gbox)
		return last;
	curr = gbox;
	while (curr->next)
		curr = curr->next;
	curr->next = last;
	return gbox;
}


