#include "drillwriter-gtk.h"
//#include "dr-sidebar-groups.h"


static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	//GtkWidget *entry_groupname;
	//GtkWidget *group_cell;
	//GtkWidget *group_cell_local;
	//GtkWidget *form_cell;
	//GtkWidget *prev_form_cell, *next_form_cell;
	GtkWidget *global_frame, *local_frame, *form_frame;
	GtkWidget *prev_frame, *next_frame;
	GtkWidget *box_global, *box_local, *box_form;
	GtkWidget *box_prev_form, *box_next_form;
	groupcell_l *group_head, *group_local_head;
	groupcell_l *form_head, *pform_head, *nform_head;
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
	GtkWidget *prev_frame, *next_frame;
	GtkWidget *box_prev_form, *box_next_form;
	groupcell_l *form_head, *pform_head, *nform_head;
	groupcell_l *group_head, *group_local_head;


	sidebargroups->priv = DR_SIDEBAR_GROUPS_GET_PRIVATE(sidebargroups);
	//sidebargroups->priv->group_cell = NULL;
	//sidebargroups->priv->group_cell_local = NULL;
	sidebargroups->priv->currset = pshow->sets->currset;

	form_head = (groupcell_l*)malloc(sizeof(groupcell_l));
	pform_head = (groupcell_l*)malloc(sizeof(groupcell_l));
	nform_head = (groupcell_l*)malloc(sizeof(groupcell_l));
	group_head = (groupcell_l*)malloc(sizeof(groupcell_l));
	group_local_head = (groupcell_l*)malloc(sizeof(groupcell_l));

	groupcell_list_init(form_head);
	groupcell_list_init(pform_head);
	groupcell_list_init(nform_head);
	groupcell_list_init(group_head);
	groupcell_list_init(group_local_head);

	sidebargroups->priv->form_head = form_head;
	sidebargroups->priv->pform_head = pform_head;
	sidebargroups->priv->nform_head = nform_head;
	sidebargroups->priv->group_head = group_head;
	sidebargroups->priv->group_local_head = group_local_head;



	button_add_group = gtk_button_new_with_label("Add group");
	gtk_widget_show(button_add_group);
	gtk_box_pack_start(GTK_BOX(sidebargroups), button_add_group, FALSE, FALSE, 5);
	g_signal_connect(button_add_group, "clicked", G_CALLBACK(add_group_gtk), NULL);


	sidebargroups->priv->global_frame = gtk_frame_new("Global groups");
	gtk_widget_show(sidebargroups->priv->global_frame);
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->global_frame, FALSE, FALSE, 0);
	//sidebargroups->priv->box_global = gtk_vbox_new(TRUE, 0);
	sidebargroups->priv->box_global = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->global_frame), sidebargroups->priv->box_global);

	sidebargroups->priv->local_frame = gtk_frame_new("Local groups");
	gtk_widget_show(sidebargroups->priv->local_frame);
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->local_frame, FALSE, FALSE, 0);
	//sidebargroups->priv->box_local = gtk_vbox_new(TRUE, 0);
	sidebargroups->priv->box_local = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->local_frame), sidebargroups->priv->box_local);

	sidebargroups->priv->form_frame = gtk_frame_new("Forms");
	gtk_widget_show(sidebargroups->priv->form_frame);
	gtk_box_pack_start(GTK_BOX(sidebargroups), sidebargroups->priv->form_frame, FALSE, FALSE, 0);
	sidebargroups->priv->box_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(sidebargroups->priv->form_frame), sidebargroups->priv->box_form);

	prev_frame = gtk_frame_new("Prev Set");
	gtk_widget_show(prev_frame);
	gtk_box_pack_start(GTK_BOX(sidebargroups), prev_frame, FALSE, FALSE, 0);
	sidebargroups->priv->prev_frame = prev_frame;
	box_prev_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(prev_frame), box_prev_form);
	sidebargroups->priv->box_prev_form = box_prev_form;

	next_frame = gtk_frame_new("Next Set");
	gtk_widget_show(next_frame);
	gtk_box_pack_start(GTK_BOX(sidebargroups), next_frame, FALSE, FALSE, 0);
	sidebargroups->priv->next_frame = next_frame;
	box_next_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(next_frame), box_next_form);
	sidebargroups->priv->box_next_form = box_next_form;


	stack_pool = NULL;
	
	return;
}



GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}


	
int dr_sidebar_groups_update_append(GtkWidget *container, groupcell_l *head, group_t **group_r, form_t **form_r, bool is_this_set,
		bool position)
{
	groupcell_c *last;
	groupcell_c *curr;
	groupcell_c *newnode;
	GtkWidget *widget;
	form_container_t *fcont;
	int index;
	int inc = 0;
	group_t *group;
	form_t *form;

	if (form_r)
	{
		form = *form_r;
		fcont = form_container_find_with_form(pshow->topforms, form);
	}
	else
	{
		form = NULL;
		fcont = NULL;
	}
	if (group_r)
		group = *group_r;
	else
		group = NULL;
	if (!form && !group)
	{
		printf("Error: No form or group!\nin groups_update_append\n");
		exit(-1);
	}
	// group/form needs to be added
	if (!is_this_set && fcont && form_container_find_form_at_index(fcont, pstate.setnum) != form 
			&&form_container_mapped_at_set(fcont, pstate.setnum))
	{
		index++;
	}
	else //if (inc >= index)
	{
		last = groupcell_list_first(head);
		newnode = (groupcell_c*)malloc(sizeof(groupcell_c));
		widget = dr_group_cell_new();

		if (group)
			dr_group_cell_set_group(widget, group);
		else
			dr_group_cell_set_form(widget, form);
		dr_group_cell_set_is_this_set(widget, is_this_set);
		gtk_widget_show(widget);
		gtk_box_pack_start(GTK_BOX(container), widget, FALSE, FALSE, 0);
		if (last)
		{
			while ((curr = groupcell_list_next(last)) != NULL)
				last = curr;
			groupcell_list_insert_after(head, newnode, last);
		}
		else
		{
			groupcell_list_insert_head(head, newnode);
		}
		// end of fixme area
		newnode->cell = widget;
	}
	inc++;
	if (group)
		group = group->next;
	else if (form)
		form = form->next;
	if (form_r)
		*form_r = form;
	if (group_r)
		*group_r = group;
	return 0;
}


//int dr_sidebar_groups_update_from(GtkWidget *container, GtkWidget **head_r, GtkWidget **last_r, group_t *group_head, form_t *form_head, bool is_this_set)
int dr_sidebar_groups_update_from(GtkWidget *container, groupcell_l *head_l, group_t *group_head, form_t *form_head, bool is_this_set)
{
	// update a list of group cells
	//group_t *group;
	//form_t *form;
	//bool is_first = false;
	//bool is_rel_first = false;
	bool rem_group = false;
	bool rem_form = false;
	bool is_local = false;
	bool form_parent_present = false;
	int index = 0;
	int type;
	group_t *currgroup;
	form_t *currform;
	form_t *form;
	group_t *group;

	groupcell_c *last;
	groupcell_c *gnext;
	last = groupcell_list_first(head_l);

	form = form_head;
	group = group_head;
	while (last)
	{
		type = dr_group_cell_get_container_type(last->cell);
		if (type == GROUP_CELL_TYPE_GROUP)
		{
			currgroup = dr_group_cell_get_group(last->cell);
			rem_group = !currgroup;
		}
		else
		{
			currgroup = NULL;
			rem_group = false;
		}

		if (type == GROUP_CELL_TYPE_FORM)
		{
			currform = dr_group_cell_get_form(last->cell);
			rem_form = !currform;
		}
		else
		{
			currform = NULL;
			rem_form = false;
		}

		is_local = dr_group_cell_get_is_this_set(last->cell);
		if (!is_local && dr_group_cell_check_form_nonlocal(last->cell))
		{
			form_parent_present = true;
		}

		// delete nodes that are present in group cells,
		// but not present in group or form list.
		if (rem_group || rem_form || form_parent_present)
		{
			// group or form was deleted (or out of scope). Delete group cell
			gtk_widget_hide(last->cell);
			g_object_ref(last->cell);
			gtk_container_remove(GTK_CONTAINER(container), last->cell);
			gnext = groupcell_list_next(last);
			groupcell_list_remove(head_l, last);
			dr_group_cell_delete_from(last->cell);
			free(last);
			last = gnext;
			form_parent_present = false;
		}
		else if ((group && currgroup && currgroup != group) || (form && currform && currform != form))
		{
			dr_sidebar_groups_flush_local(container, head_l);
			last = NULL;
			form = form_head;
			group = group_head;
		}
		else
		{
			// traverse to the next node
			//lastcurr = last->cell;
			//last = gnext->group_cell_container;
			//last = dr_group_cell_get_next(lastcurr);
			last = groupcell_list_next(last);
			index++;
			if (group)
				group = group->next;
			if (form)
				form = form->next;

		}
	}
	// make sure if list is now empty, node is shown as empty
	/*
	if (index)
		last = lastcurr;
	else
		last = NULL;
		*/

	while (group || form)
	{
		dr_sidebar_groups_update_append(container, head_l, &group, &form, is_this_set, 1);
	}
	/*
	while (group || form)
	{
		// added a new group
		// add another sidebar ref
		fcont = form_container_find_with_form(pshow->topforms, form);
		if (!is_this_set && fcont && form_container_find_form_at_index(fcont, pstate.setnum) != form 
				&&form_container_mapped_at_set(fcont, pstate.setnum))
		{
			index++;
		}
		else if (inc >= index)
		{
			if (!last)
			{
				last = dr_group_cell_new();
				//lsidebargroups->priv->group_cell = last;
				head = last;
				if (group)
					dr_group_cell_set_group(last, group);
				else
					dr_group_cell_set_form(last, form);
			}
			else
			{
				last = dr_group_cell_append(last, group, form);
				last = dr_group_cell_get_next(last);
			}
			dr_group_cell_set_is_this_set(last, is_this_set);
			gtk_box_pack_start(GTK_BOX(container), last, FALSE, FALSE, 0);
			gtk_widget_show(last);
		}
		inc++;
		if (group)
			group = group->next;
		else if (form)
			form = form->next;
	}
	*/


	return index;
}


//void dr_sidebar_groups_flush_local(GtkWidget *container, GtkWidget **cell_r)
void dr_sidebar_groups_flush_local(GtkWidget *container, groupcell_l *cell_head)
{
	//GtkWidget *last;
	groupcell_c *last;
	/*
	if (!cell_r)
		return;
	last = *cell_r;
	*/

	//lastcurr = last;
	//while (last)
	while (!groupcell_list_empty(cell_head))
	{
		last = groupcell_list_first(cell_head);
		groupcell_list_remove(cell_head, last);
		gtk_widget_hide(last->cell);
		g_object_ref(last->cell);
		gtk_container_remove(GTK_CONTAINER(container), last->cell);
		dr_group_cell_delete_from(last->cell);
		free(last);
	}
	//*cell_r = last;
	return;
}


void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	DrSidebarGroups *lsidebargroups;

	lsidebargroups = (DrSidebarGroups*)sidebargroups;

	if (pshow->sets->currset != lsidebargroups->priv->currset)
	{
		// flush local groups and forms out
		//last = lsidebargroups->priv->group_cell_local;

		// groups
		//dr_sidebar_groups_flush_local(lsidebargroups->priv->box_local, &lsidebargroups->priv->group_cell_local);
		dr_sidebar_groups_flush_local(lsidebargroups->priv->box_local, lsidebargroups->priv->group_local_head);

		// forms
		//dr_sidebar_groups_flush_local(lsidebargroups->priv->box_form, &lsidebargroups->priv->form_cell);
		dr_sidebar_groups_flush_local(lsidebargroups->priv->box_form, lsidebargroups->priv->form_head);

		// forms from previous set
		//dr_sidebar_groups_flush_local(lsidebargroups->priv->box_prev_form, &lsidebargroups->priv->prev_form_cell);
		dr_sidebar_groups_flush_local(lsidebargroups->priv->box_prev_form, lsidebargroups->priv->pform_head);

		// forms from next set
		//dr_sidebar_groups_flush_local(lsidebargroups->priv->box_next_form, &lsidebargroups->priv->next_form_cell);
		dr_sidebar_groups_flush_local(lsidebargroups->priv->box_next_form, lsidebargroups->priv->nform_head);

		lsidebargroups->priv->currset = pshow->sets->currset;
	}

	// Global Groups
	dr_sidebar_groups_update_from(lsidebargroups->priv->box_global, lsidebargroups->priv->group_head, pshow->topgroups, NULL, true);


	// Local Groups
	dr_sidebar_groups_update_from(lsidebargroups->priv->box_local, lsidebargroups->priv->group_local_head, pshow->sets->currset->groups, NULL, true);

	// Forms
	dr_sidebar_groups_update_from(lsidebargroups->priv->box_form, lsidebargroups->priv->form_head, NULL, pshow->sets->currset->forms, true);

	// Previous Set Forms
	if (pshow->sets->currset->prev)
	{
		dr_sidebar_groups_update_from(lsidebargroups->priv->box_prev_form, lsidebargroups->priv->pform_head, 
				NULL, pshow->sets->currset->prev->forms, false);
	}

	// Next Set Forms
	if (pshow->sets->currset->next)
	{
		dr_sidebar_groups_update_from(lsidebargroups->priv->box_next_form, lsidebargroups->priv->nform_head,
				NULL, pshow->sets->currset->next->forms, false);
	}


	printf("%i\n", groupcell_list_empty(lsidebargroups->priv->form_head));
	if (groupcell_list_empty(lsidebargroups->priv->group_head))
	{
		gtk_widget_hide(lsidebargroups->priv->box_global);
		gtk_widget_hide(lsidebargroups->priv->global_frame);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_global);
		gtk_widget_show(lsidebargroups->priv->global_frame);
	}

	if (groupcell_list_empty(lsidebargroups->priv->group_local_head))
	{
		gtk_widget_hide(lsidebargroups->priv->local_frame);
		gtk_widget_hide(lsidebargroups->priv->box_local);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_local);
		gtk_widget_show(lsidebargroups->priv->local_frame);
	}
	if (groupcell_list_empty(lsidebargroups->priv->form_head))
	{
		gtk_widget_hide(lsidebargroups->priv->form_frame);
		gtk_widget_hide(lsidebargroups->priv->box_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_form);
		gtk_widget_show(lsidebargroups->priv->form_frame);
	}
	if (groupcell_list_empty(lsidebargroups->priv->pform_head))
	{
		gtk_widget_hide(lsidebargroups->priv->prev_frame);
		gtk_widget_hide(lsidebargroups->priv->box_prev_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->prev_frame);
		gtk_widget_show(lsidebargroups->priv->box_prev_form);
	}
	if (groupcell_list_empty(lsidebargroups->priv->nform_head))
	{
		gtk_widget_hide(lsidebargroups->priv->next_frame);
		gtk_widget_hide(lsidebargroups->priv->box_next_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->next_frame);
		gtk_widget_show(lsidebargroups->priv->box_next_form);
	}
	return;
}


void groupcell_list_init(groupcell_l *head)
{
	head->list_first = NULL;
	return;
}

int groupcell_list_empty(groupcell_l *head)
{
	if (head->list_first == NULL)
		return 1;
	return 0;
}

groupcell_c *groupcell_list_first(groupcell_l *head)
{
	return head->list_first;
}

void groupcell_list_remove(groupcell_l *head, groupcell_c *last)
{
	groupcell_c *prev, *next;
	if (!last)
		return;
	prev = last->prev;
	next = last->next;
	last->prev = NULL;
	last->next = NULL;
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	if (head->list_first == last)
		head->list_first = next;
	return;
}

void groupcell_list_insert_after(groupcell_l *head, groupcell_c *curr, groupcell_c *last)
{
	groupcell_c *next;
	if (!last)
	{
		groupcell_list_append(head, curr);
		return;
	}
	next = last->next;
	if (next)
		next->prev = curr;
	curr->next = next;
	curr->prev = last;
	last->next = curr;
	return;
}

void groupcell_list_insert_before(groupcell_l *head, groupcell_c *curr, groupcell_c *last)
{
	groupcell_c *prev;
	if (!last)
	{
		groupcell_list_insert_head(head, curr);
		return;
	}
	prev = last->prev;
	if (prev)
		prev->next = curr;
	curr->next = last;
	curr->prev = prev;
	last->prev = curr;
	if (head->list_first == last)
		head->list_first = curr;
	return;
}


void groupcell_list_insert_head(groupcell_l *head, groupcell_c *curr)
{
	groupcell_c *next;
	next = head->list_first;
	if (next)
		next->prev = curr;
	curr->next = next;
	curr->prev = NULL;
	head->list_first = curr;
	return;
}

void groupcell_list_append(groupcell_l *head, groupcell_c *curr)
{
	groupcell_c *last;
	last = groupcell_list_first(head);
	if (!last)
	{
		groupcell_list_insert_head(head, curr);
		return;
	}
	while (last->next)
		last = last->next;
	last->next = curr;
	curr->prev = last;
	curr->next = NULL;
	return;
}

groupcell_c *groupcell_list_next(groupcell_c *curr)
{
	if (!curr)
		return NULL;
	return curr->next;
}

groupcell_c *groupcell_list_prev(groupcell_c *curr)
{
	if (!curr)
		return NULL;
	return curr->prev;
}
