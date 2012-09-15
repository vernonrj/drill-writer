#include "drillwriter-gtk.h"
//#include "dr-sidebar-groups.h"


static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	//GtkWidget *entry_groupname;
	GtkWidget *group_cell;
	GtkWidget *group_cell_local;
	GtkWidget *form_cell;
	GtkWidget *prev_form_cell;
	GtkWidget *global_frame, *local_frame, *form_frame;
	GtkWidget *prev_frame;
	GtkWidget *box_global, *box_local, *box_form;
	GtkWidget *box_prev_form;
	set_t *currset;
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
	GtkWidget *prev_frame;
	GtkWidget *box_prev_form;


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

	prev_frame = gtk_frame_new("Prev Set");
	gtk_box_pack_start(GTK_BOX(sidebargroups), prev_frame, FALSE, FALSE, 0);
	sidebargroups->priv->prev_frame = prev_frame;
	box_prev_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(prev_frame), box_prev_form);
	sidebargroups->priv->box_prev_form = box_prev_form;

	stack_pool = NULL;
	
	return;
}



GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}

int dr_sidebar_groups_update_from(GtkWidget *container, GtkWidget **head_r, GtkWidget **last_r)
{
	// update a list of group cells
	GtkWidget *head;
	GtkWidget *last;
	GtkWidget *gnext;
	//group_t *group;
	//form_t *form;
	last = *last_r;
	head = *head_r;
	bool is_first = false;
	GtkWidget *lastcurr = last;
	head = last;
	bool is_rel_first = false;
	bool rem_group = false;
	bool rem_form = false;
	int index = 0;
	while (last)
	{
		rem_group = (dr_group_cell_get_container_type(last) == GROUP_CELL_TYPE_GROUP) && !dr_group_cell_get_group(last);
		rem_form = (dr_group_cell_get_container_type(last) == GROUP_CELL_TYPE_FORM) && !dr_group_cell_get_form(last);
		// delete nodes that are present in group cells,
		// but not present in group or form list.
		// Currently, only a list of global groups and groups, forms from the current set are tracked
		if (rem_group || rem_form)
		{
			// group or form was deleted (or out of scope). Delete group cell
			if (last == head)
				is_first = true;
			if (last == lastcurr)
				is_rel_first = true;
			gtk_widget_hide(last);
			g_object_ref(last);
			gtk_container_remove(GTK_CONTAINER(container), last);
			gnext = dr_group_cell_delete_from(last, lastcurr);
			if (is_first)
			{
				head = gnext;
				is_first = false;
			}
			if (is_rel_first)
			{
				lastcurr = last;
				is_rel_first = false;
			}
			last = gnext;
		}
		else
		{
			// traverse to the next node
			lastcurr = last;
			last = dr_group_cell_get_next(lastcurr);
			index++;
		}
	}
	// make sure if list is now empty, node is shown as empty
	if (index)
		last = lastcurr;
	else
		last = NULL;
	*last_r = last;
	*head_r = head;
	return index;
}


void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	DrSidebarGroups *lsidebargroups;

	lsidebargroups = (DrSidebarGroups*)sidebargroups;
	GtkWidget *last;
	GtkWidget *lastcurr;
	GtkWidget *gnext;
	group_t *group = pshow->topgroups;
	form_t *form; 
	int index = 0;
	int inc;


	// Global Groups
	last = lsidebargroups->priv->group_cell;
	index = dr_sidebar_groups_update_from(lsidebargroups->priv->box_global, &lsidebargroups->priv->group_cell, &last);
	lastcurr = last;
	inc = 0;
	while (group)
	{
		// added a new group
		// add another sidebar ref
		if (inc >= index)
		{
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
		}
		inc++;
		group = group->next;
	}

	// Local Groups
	last = lsidebargroups->priv->group_cell_local;
	group = pshow->sets->currset->groups;
	if (pshow->sets->currset != lsidebargroups->priv->currset)
	{
		// flush local groups and forms out

		// groups
		lastcurr = last;
		while (last)
		{
			gtk_widget_hide(last);
			gtk_container_remove(GTK_CONTAINER(lsidebargroups->priv->box_local), last);
			gnext = dr_group_cell_delete_from(last, lastcurr);
			last = gnext;
		}
		lsidebargroups->priv->group_cell_local = NULL;

		// forms
		last = lsidebargroups->priv->form_cell;
		lastcurr = last;
		while (last)
		{
			gtk_widget_hide(last);
			g_object_ref(last);
			gtk_container_remove(GTK_CONTAINER(lsidebargroups->priv->box_form), last);
			gnext = dr_group_cell_delete_from(last, lastcurr);
			last = gnext;
		}
		lsidebargroups->priv->form_cell = NULL;

		// forms from previous set
		last = lsidebargroups->priv->prev_form_cell;
		lastcurr = last;
		while (last)
		{
			gtk_widget_hide(last);
			g_object_ref(last);
			gtk_container_remove(GTK_CONTAINER(lsidebargroups->priv->box_prev_form), last);
			gnext = dr_group_cell_delete_from(last, lastcurr);
			last = gnext;
		}
		lsidebargroups->priv->prev_form_cell = NULL;

		last = NULL;
		lsidebargroups->priv->currset = pshow->sets->currset;
	}
	index = dr_sidebar_groups_update_from(lsidebargroups->priv->box_local, &lsidebargroups->priv->group_cell_local, &last);
	inc = 0;
	while (group)
	{
		// added a new group
		// add another sidebar ref
		if(inc >= index)
		{
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
		}
		inc++;
		group = group->next;
	}

	// Forms
	last = lsidebargroups->priv->form_cell;
	form = pshow->sets->currset->forms;
	index = dr_sidebar_groups_update_from(lsidebargroups->priv->box_form, &lsidebargroups->priv->form_cell, &last);
	inc = 0;
	while(form)
	{
		// added a new form
		// add another sidebar ref
		if(inc >= index)
		{
			if (!lsidebargroups->priv->form_cell)
			{
				last = dr_group_cell_new();
				lsidebargroups->priv->form_cell = last;
				dr_group_cell_set_form(last, form);
			}
			else if (!last)
			{
				printf("ping\n");
			}
			else
			{
				last = dr_group_cell_append(last, NULL, form);
				last = dr_group_cell_get_next(last);
			}
			gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_form), last, FALSE, FALSE, 0);
			gtk_widget_show(last);
		}
		form = form->next;
		inc++;
	}

	/*
	// Previous Set Forms
	last = lsidebargroups->priv->prev_form_cell;
	if (pshow->sets->currset->prev)
		form = pshow->sets->currset->prev->forms;
	else
		form = NULL;
	dr_sidebar_groups_update_from(lsidebargroups->priv->prev_form_cell, &last);
	if (!last)
		lsidebargroups->priv->prev_form_cell = NULL;
	while (form)
	{
		// added a new prev form
		// add another sidebar ref
		if (!last)
		{
			last = dr_group_cell_new();
			lsidebargroups->priv->prev_form_cell = last;
			dr_group_cell_set_form(last, form);
		}
		else
		{
			last = dr_group_cell_append(last, NULL, form);
			last = dr_group_cell_get_next(last);
		}
		gtk_box_pack_start(GTK_BOX(lsidebargroups->priv->box_prev_form), last, FALSE, FALSE, 0);
		dr_group_cell_set_is_this_set(last, false);
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
	if (false && !lsidebargroups->priv->form_cell)
	{
		gtk_widget_hide(lsidebargroups->priv->form_frame);
		gtk_widget_hide(lsidebargroups->priv->box_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_form);
		gtk_widget_show(lsidebargroups->priv->form_frame);
	}
	/*
	if (!lsidebargroups->priv->prev_form_cell)
	{
		gtk_widget_hide(lsidebargroups->priv->prev_frame);
		gtk_widget_hide(lsidebargroups->priv->box_prev_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->prev_frame);
		gtk_widget_show(lsidebargroups->priv->box_prev_form);
	}
	*/
	return;
}




