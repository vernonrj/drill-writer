#include "drillwriter-gtk.h"
#include "dr-sidebar-groups.h"
#include "dr-group-cell.h"
#include "../dr_forms.h"
#include "../dr_form_parent.h"


static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	GtkWidget *global_frame, *local_frame, *form_frame;
	GtkWidget *prev_frame, *next_frame;
	GtkWidget *box_global, *box_local, *box_form;
	GtkWidget *box_prev_form, *box_next_form;
	groupcell_l group_head, group_local_head;
	groupcell_l form_head, pform_head, nform_head;
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
	/*
	groupcell_l *form_head, *pform_head, *nform_head;
	groupcell_l *group_head, *group_local_head;
	*/


	sidebargroups->priv = DR_SIDEBAR_GROUPS_GET_PRIVATE(sidebargroups);
	sidebargroups->priv->currset = pshow->sets->currset;

	/*
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


	LIST_INIT(form_head);
	LIST_INIT(pform_head);
	LIST_INIT(nform_head);
	LIST_INIT(group_head);
	LIST_INIT(group_local_head);

	sidebargroups->priv->form_head = form_head;
	sidebargroups->priv->pform_head = pform_head;
	sidebargroups->priv->nform_head = nform_head;
	sidebargroups->priv->group_head = group_head;
	sidebargroups->priv->group_local_head = group_local_head;
	*/

	LIST_INIT(&sidebargroups->priv->form_head);
	LIST_INIT(&sidebargroups->priv->pform_head);
	LIST_INIT(&sidebargroups->priv->nform_head);
	LIST_INIT(&sidebargroups->priv->group_head);
	LIST_INIT(&sidebargroups->priv->group_local_head);


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


	//stack_pool = NULL;
	
	return;
}



GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}


	
groupcell_l *dr_sidebar_groups_update_append(GtkWidget *container, groupcell_l *head, group_t **group_r, form_child_t **form_r, bool is_this_set,
		bool position)
{
	struct group_cell_container *last;
	struct group_cell_container *curr;
	struct group_cell_container *newnode;
	GtkWidget *widget;
	form_parent_t *fparent;
	group_t *group;
	form_child_t *form;


	if (form_r)
	{
		form = *form_r;
		//fparent = form_parent_find_with_form(pshow->topforms, form);
		fparent = form_child_get_parent(form);
	}
	else
	{
		form = NULL;
		fparent = NULL;
	}
	if (group_r)
		group = *group_r;
	else
		group = NULL;
	// group/form needs to be added
	if (!is_this_set && fparent && form_parent_find_form_at_index(fparent, pstate.setnum) != form 
			&&form_parent_mapped_at_set(fparent, pstate.setnum))
	{
	}
	else
	{
		//last = groupcell_list_first(head);
		newnode = (struct group_cell_container*)malloc(sizeof(struct group_cell_container));
		newnode->next = newnode->prev = NULL;
		widget = dr_group_cell_new();
		newnode->cell = widget;

		if (group)
			dr_group_cell_set_group(widget, group);
		else if (form)
			dr_group_cell_set_form(widget, form);
		else
			printf("warning: no group or form!\n");
		dr_group_cell_set_is_this_set(widget, is_this_set);
		gtk_widget_show(widget);
		gtk_box_pack_start(GTK_BOX(container), widget, FALSE, FALSE, 0);
		if (!LIST_EMPTY(head))
		{
			last = LIST_FIRST(head);
			//while ((curr = groupcell_list_next(last)) != NULL)
			do
			{
				curr = LIST_NEXT(last, groupcell_entries);
				if (curr)
					last = curr;
			} while (curr);
			/*
			while ((curr = LIST_NEXT(last, groupcell_entries)) != NULL)
				last = curr;
				*/
			//groupcell_list_insert_after(head, newnode, last);
			LIST_INSERT_AFTER(last, newnode, groupcell_entries);
		}
		else
		{
			//groupcell_list_insert_head(head, newnode);
			LIST_INSERT_HEAD(head, newnode, groupcell_entries);
		}
	}
	if (group)
		group = group_get_next(group);
	else if (form)
	{
		//form = form->next;
		form = form_child_get_next(form);
	}
	if (form_r)
		*form_r = form;
	if (group_r)
		*group_r = group;
	return head;
}


groupcell_l *dr_sidebar_groups_update_from(GtkWidget *container, groupcell_l *head_l, group_t *group_head, form_child_t *form_head, bool is_this_set)
{
	// update a list of group cells
	bool rem_group = false;
	bool rem_form = false;
	bool is_local = false;
	bool form_parent_present = false;
	int index = 0;
	int type;
	group_t *currgroup;
	form_child_t *currform;
	form_child_t *form;
	group_t *group;

	struct group_cell_container *last;
	struct group_cell_container *gnext;
	//last = groupcell_list_first(head_l);
	last = LIST_FIRST(head_l);

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
			//gnext = groupcell_list_next(last);
			//groupcell_list_remove(head_l, last);
			gnext = LIST_NEXT(last, groupcell_entries);
			LIST_REMOVE(last, groupcell_entries);
			dr_group_cell_delete_from(last->cell);
			free(last);
			last = gnext;
			form_parent_present = false;
		}
		else if ((group && currgroup && currgroup != group) || (form && currform && currform != form))
		{
			head_l = dr_sidebar_groups_flush_local(container, head_l);
			last = NULL;
			form = form_head;
			group = group_head;
		}
		else
		{
			// traverse to the next node
			//last = groupcell_list_next(last);
			last = LIST_NEXT(last, groupcell_entries);
			index++;
			if (group)
				group = group_get_next(group);
			if (form)
			{
				//form = form->next;
				form = form_child_get_next(form);
			}

		}
	}
	// make sure if list is now empty, node is shown as empty

	while (group || form)
	{
		head_l = dr_sidebar_groups_update_append(container, head_l, &group, &form, is_this_set, 1);
	}


	return head_l;
}


groupcell_l *dr_sidebar_groups_flush_local(GtkWidget *container, groupcell_l *cell_head)
{
	struct group_cell_container *last;
	//while (!groupcell_list_empty(cell_head))
	while (!LIST_EMPTY(cell_head))
	{
		//last = groupcell_list_first(cell_head);
		//groupcell_list_remove(cell_head, last);
		last = LIST_FIRST(cell_head);
		LIST_REMOVE(last, groupcell_entries);
		gtk_widget_hide(last->cell);
		g_object_ref(last->cell);
		gtk_container_remove(GTK_CONTAINER(container), last->cell);
		dr_group_cell_delete_from(last->cell);
		free(last);
	}
	return cell_head;
}


void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	set_t *prevset;
	set_t *nextset;
	int setnum = pstate.setnum;
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	DrSidebarGroups *lsidebargroups;

	lsidebargroups = (DrSidebarGroups*)sidebargroups;

	if (pshow->sets->currset != lsidebargroups->priv->currset)
	{
		// flush local groups and forms out

		// groups
		lsidebargroups->priv->group_local_head = *dr_sidebar_groups_flush_local(lsidebargroups->priv->box_local, &lsidebargroups->priv->group_local_head);

		// forms
		lsidebargroups->priv->form_head = *dr_sidebar_groups_flush_local(lsidebargroups->priv->box_form, &lsidebargroups->priv->form_head);

		// forms from previous set
		lsidebargroups->priv->pform_head = *dr_sidebar_groups_flush_local(lsidebargroups->priv->box_prev_form, &lsidebargroups->priv->pform_head);

		// forms from next set
		lsidebargroups->priv->nform_head = *dr_sidebar_groups_flush_local(lsidebargroups->priv->box_next_form, &lsidebargroups->priv->nform_head);

		lsidebargroups->priv->currset = pshow->sets->currset;
	}

	// Global Groups
	lsidebargroups->priv->group_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_global, &lsidebargroups->priv->group_head, pshow->topgroups, NULL, true);


	// Local Groups
	lsidebargroups->priv->group_local_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_local, &lsidebargroups->priv->group_local_head, pshow->sets->currset->groups, NULL, true);

	// Forms
	lsidebargroups->priv->form_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_form, &lsidebargroups->priv->form_head, NULL, pshow->sets->currset->forms, true);

	// Previous Set Forms
	//if (pshow->sets->currset->prev)
	if ((prevset = set_get_prev(pshow->sets, setnum)) != NULL)
	{
		/*
		lsidebargroups->priv->pform_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_prev_form, &lsidebargroups->priv->pform_head, 
				NULL, pshow->sets->currset->prev->forms, false);
				*/
		lsidebargroups->priv->pform_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_prev_form, &lsidebargroups->priv->pform_head, 
				NULL, prevset->forms, false);
	}

	// Next Set Forms
	//if (pshow->sets->currset->next)
	if ((nextset = set_get_next(pshow->sets, setnum)) != NULL)
	{
		lsidebargroups->priv->nform_head = *dr_sidebar_groups_update_from(lsidebargroups->priv->box_next_form, &lsidebargroups->priv->nform_head,
				NULL, nextset->forms, false);
	}


	//if (groupcell_list_empty(lsidebargroups->priv->group_head))
	if (LIST_EMPTY(&lsidebargroups->priv->group_head))
	{
		gtk_widget_hide(lsidebargroups->priv->box_global);
		gtk_widget_hide(lsidebargroups->priv->global_frame);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_global);
		gtk_widget_show(lsidebargroups->priv->global_frame);
	}

	//if (groupcell_list_empty(lsidebargroups->priv->group_local_head))
	if (LIST_EMPTY(&lsidebargroups->priv->group_local_head))
	{
		gtk_widget_hide(lsidebargroups->priv->local_frame);
		gtk_widget_hide(lsidebargroups->priv->box_local);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_local);
		gtk_widget_show(lsidebargroups->priv->local_frame);
	}
	//if (groupcell_list_empty(lsidebargroups->priv->form_head))
	if (LIST_EMPTY(&lsidebargroups->priv->form_head))
	{
		gtk_widget_hide(lsidebargroups->priv->form_frame);
		gtk_widget_hide(lsidebargroups->priv->box_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->box_form);
		gtk_widget_show(lsidebargroups->priv->form_frame);
	}
	//if (groupcell_list_empty(lsidebargroups->priv->pform_head))
	if (LIST_EMPTY(&lsidebargroups->priv->pform_head))
	{
		gtk_widget_hide(lsidebargroups->priv->prev_frame);
		gtk_widget_hide(lsidebargroups->priv->box_prev_form);
	}
	else
	{
		gtk_widget_show(lsidebargroups->priv->prev_frame);
		gtk_widget_show(lsidebargroups->priv->box_prev_form);
	}
	//if (groupcell_list_empty(lsidebargroups->priv->nform_head))
	if (LIST_EMPTY(&lsidebargroups->priv->nform_head))
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


/*
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

struct group_cell_container *groupcell_list_first(groupcell_l *head)
{
	return head->list_first;
}

void groupcell_list_remove(groupcell_l *head, struct group_cell_container *last)
{
	struct group_cell_container *prev, *next;
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

void groupcell_list_insert_after(groupcell_l *head, struct group_cell_container *curr, struct group_cell_container *last)
{
	struct group_cell_container *next;
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

void groupcell_list_insert_before(groupcell_l *head, struct group_cell_container *curr, struct group_cell_container *last)
{
	struct group_cell_container *prev;
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


void groupcell_list_insert_head(groupcell_l *head, struct group_cell_container *curr)
{
	struct group_cell_container *next;
	next = head->list_first;
	if (next)
		next->prev = curr;
	curr->next = next;
	curr->prev = NULL;
	head->list_first = curr;
	return;
}

void groupcell_list_append(groupcell_l *head, struct group_cell_container *curr)
{
	struct group_cell_container *last;
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

struct group_cell_container *groupcell_list_next(struct group_cell_container *curr)
{
	if (!curr)
		return NULL;
	return curr->next;
}

struct group_cell_container *groupcell_list_prev(struct group_cell_container *curr)
{
	if (!curr)
		return NULL;
	return curr->prev;
}
*/
