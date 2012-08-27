#include "d_gtk.h"
//#include "dr-sidebar-groups.h"

static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	GtkWidget *entry_groupname;
	group_box_t *group_box;
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
	GtkWidget *entry_groupname;
	GtkWidget *button_add_group;

	char groupname_buf[20];

	sidebargroups->priv = DR_SIDEBAR_GROUPS_GET_PRIVATE(sidebargroups);
	sidebargroups->priv->group_box = NULL;

	button_add_group = gtk_button_new_with_label("Add group");
	gtk_widget_show(button_add_group);
	gtk_box_pack_start(GTK_BOX(sidebargroups), button_add_group, FALSE, FALSE, 5);


	sprintf(groupname_buf, "Empty");
	entry_groupname = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_groupname), 5);
	//g_signal_connect (entry_groupname, "activate", G_CALLBACK (goto_perf), entry_groupname);
	gtk_entry_set_text (GTK_ENTRY (entry_groupname), groupname_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_groupname), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_groupname), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_groupname, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (sidebargroups), entry_groupname, FALSE, TRUE, 5);
	gtk_widget_show(entry_groupname);

	sidebargroups->priv->entry_groupname = entry_groupname;
	return;
}

GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}

void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	char groupname_buf[20];
	DrSidebarGroups *lsidebargroups;

	lsidebargroups = (DrSidebarGroups*)sidebargroups;
	group_box_t *gbox = lsidebargroups->priv->group_box;
	group_box_t *last = gbox;
	group_box_t *curr;
	group_t *group = pshow->topgroups;

	snprintf(groupname_buf, 19, "Empty");
	gtk_entry_set_text(GTK_ENTRY(lsidebargroups->priv->entry_groupname), groupname_buf);

	while (last && last->next && group && group->next)
	{
		if (group != last->group)
		{
			// add group
			// or delete group
		}
		last = last->next;
		group = group->next;
	}
	if (!group || !last)
	{
		if (!group)
		{
			// no groups anymore
			// delete sidebar refs
		}
		else if (!last)
		{
			// added first group
			// add first sidebar ref
		}
	}
	else if (!last->next && !group->next)
	{
		// both lined up
	}
	else if (!last->next || !group->next)
	{
		if (!last->next && group->next)
		{
			// added a new group
			// add another sidebar ref
		}
		else if (!group->next && last->next)
		{
			// deleted a group
			// remove sidebar ref
		}
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


