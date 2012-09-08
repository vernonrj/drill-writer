#include "drillwriter-gtk.h"
//#include "dr-sidebar-groups.h"


static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	//GtkWidget *entry_groupname;
	GtkWidget *group_cell;
	GtkWidget *group_cell_local;
	GtkWidget *form_cell;
	GtkWidget *global_frame, *local_frame, *form_frame;
	GtkWidget *box_global, *box_local, *box_form;
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
	//sidebargroups->priv->box_global = gtk_vbox_new(TRUE, 0);
	sidebargroups->priv->box_global = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->global_frame), sidebargroups->priv->box_global);

	sidebargroups->priv->local_frame = gtk_frame_new("Local groups");
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->local_frame, FALSE, FALSE, 0);
	//sidebargroups->priv->box_local = gtk_vbox_new(TRUE, 0);
	sidebargroups->priv->box_local = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->local_frame), sidebargroups->priv->box_local);

	sidebargroups->priv->form_frame = gtk_frame_new("Forms");
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->form_frame, FALSE, FALSE, 0);
	sidebargroups->priv->box_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->form_frame), sidebargroups->priv->box_form);


	return;
}

GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}

DrSidebarGroups *dr_sidebar_groups_update_from(DrSidebarGroups *lsidebargroups, GtkWidget **last_r, group_t **group_r, form_t **form_r)
{
	GtkWidget *last;
	group_t *group;
	form_t *form;
	last = *last_r;
	if (group_r)
		group = *group_r;
	else
		group = NULL;
	if (form_r)
		form = *form_r;
	else
		form = NULL;
	GtkWidget *lastcurr = last;
	bool listEmpty = false;
	while (last && (group || form))
	{
		if ((dr_group_cell_get_container_type(last) == GROUP_CELL_TYPE_GROUP) && !dr_group_cell_get_group(last))
		{
			gtk_widget_hide(last);
			last = dr_group_cell_delete_from(last, lastcurr);
		}
		else if ((dr_group_cell_get_container_type(last) == GROUP_CELL_TYPE_FORM) && !dr_group_cell_get_form(last))
		{
			gtk_widget_hide(last);
			last = dr_group_cell_delete_from(last, lastcurr);
		}
		else
		{
			lastcurr = last;
			last = dr_group_cell_get_next(lastcurr);
			//lastgroup = group;
			if (group)
				group = group->next;
			else if (form)
				form = form->next;
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
		else if (!dr_group_cell_get_form(last))
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
	if (group_r)
		*group_r = group;
	if (form_r)
		*form_r = form;
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
	form_t *form = pshow->sets->currset->forms;


	lsidebargroups = dr_sidebar_groups_update_from(lsidebargroups, &last, &group, NULL);
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
			last = dr_group_cell_append(last, group, NULL);
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


	lsidebargroups = dr_sidebar_groups_update_from(lsidebargroups, &last, &group, NULL);
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
			last = dr_group_cell_append(last, group, NULL);
			last = dr_group_cell_get_next(last);
		}
		gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_local), last, FALSE, FALSE, 0);
		gtk_widget_show(last);
		group = group->next;
	}


	/*
	lsidebargroups = dr_sidebar_groups_update_from(lsidebargroups, &last, NULL, &form);
	if (!last)
		lsidebargroups->priv->form_cell = NULL;
	while(form)
	{
		// added a new form
		// add another sidebar ref
		if (!last)
		{
			last = dr_group_cell_new();
			lsidebargroups->priv->form_cell = last;
			dr_group_cell_set_form(last, form);
		}
		else
		{
			last = dr_group_cell_append(last, NULL, form);
			last = dr_group_cell_get_next(last);
		}
		gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_form), last, FALSE, FALSE, 0);
		gtk_widget_show(last);
		form = form->next;
	}
	*/


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
	if (!lsidebargroups->priv->form_cell)
	{
		gtk_widget_hide(lsidebargroups->priv->form_frame);
		gtk_widget_hide(lsidebargroups->priv->box_form);
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


