#include "d_gtk.h"
//#include "dr-sidebar-groups.h"

static void dr_group_cell_class_init(DrGroupCellClass *klass);
static void dr_group_cell_init(DrGroupCell *groupcell);

struct _DrGroupCellPrivate {
	GtkWidget *name_entry;
	GtkWidget *button_add;
	GtkWidget *button_del;
	GtkWidget *button_name;
	group_t *group;
	DrGroupCell *next;
};

G_DEFINE_TYPE (DrGroupCell, dr_group_cell, GTK_TYPE_HBOX)

#define DR_GROUP_CELL_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_GROUP_CELL_TYPE, DrGroupCellPrivate))

static void dr_group_cell_class_init(DrGroupCellClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrGroupCellPrivate));
}

static void group_cell_select_group(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	select_group_gtk(widget, groupcell->priv->group);
	return;
}


static void select_add_to_group_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	select_add_multiple(&groupcell->priv->group->selects, &pstate.select, false);
	return;
}


static void dr_group_cell_remove_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	pshow->topgroups = group_remove_from(groupcell->priv->group, pshow->topgroups);
	groupcell->priv->group = NULL;
	dr_sidebar_update((DrSidebar*)sidebar);
}




static void dr_group_cell_init(DrGroupCell *groupcell)
{
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	GtkWidget *entry_groupname;
	GtkWidget *button;
	GtkWidget *image;

	char groupname_buf[20];

	groupcell->priv = DR_GROUP_CELL_GET_PRIVATE(groupcell);
	groupcell->priv->next = NULL;

	button = gtk_button_new();
	image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_widget_show(image);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(groupcell), button, FALSE, FALSE, 0);
	g_signal_connect(button, "clicked", G_CALLBACK(select_add_to_group_cell), groupcell);
	groupcell->priv->button_add = button;

	button = gtk_button_new();
	image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_widget_show(image);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(groupcell), button, FALSE, FALSE, 0);
	g_signal_connect(button, "clicked", G_CALLBACK(dr_group_cell_remove_cell), groupcell);
	groupcell->priv->button_del = button;


	button = gtk_button_new_with_label("foo");
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_HALF);
	g_signal_connect(button, "clicked", G_CALLBACK(group_cell_select_group), groupcell);
	gtk_box_pack_start(GTK_BOX(groupcell), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	groupcell->priv->button_name = button;



	/*
	sprintf(groupname_buf, "Empty");
	groupcell->priv->name_entry = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (groupcell->priv->name_entry), 5);
	//g_signal_connect (entry_groupname, "activate", G_CALLBACK (goto_perf), entry_groupname);
	gtk_entry_set_text (GTK_ENTRY (groupcell->priv->name_entry), groupname_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (groupcell->priv->name_entry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (groupcell->priv->name_entry), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_groupname, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (groupcell), groupcell->priv->name_entry, FALSE, TRUE, 5);
	gtk_widget_show(groupcell->priv->name_entry);
	*/

	return;
}

GtkWidget *dr_group_cell_new(void)
{
	return GTK_WIDGET(g_object_new(dr_group_cell_get_type(), NULL));
}




group_t *dr_group_cell_get_group(GtkWidget *grpcell)
{
	DrGroupCell *groupcell = (DrGroupCell*)grpcell;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	return groupcell->priv->group;
}


void dr_group_cell_set_group(GtkWidget *groupcell_widget, group_t *group)
{
	select_t *select;
	group_t *newgroup;
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	groupcell->priv->group = group;
	return;
}

GtkWidget *dr_group_cell_get_next(GtkWidget *groupcell_widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	return (GtkWidget*)groupcell->priv->next;
}

bool dr_group_cell_has_next(GtkWidget *groupcell_widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	if (!IS_GROUP_CELL(groupcell))
		return false;
	return (groupcell->priv->next != NULL);
}

GtkWidget *dr_group_cell_add(GtkWidget *groupcell_widget, group_t *group)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	GtkWidget *last;
	last = dr_group_cell_new();
	dr_group_cell_set_group(last, group);
	if (!groupcell)
		return last;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	((DrGroupCell*)last)->priv->next = groupcell;
	return last;
}

GtkWidget *dr_group_cell_append(GtkWidget *widget, group_t *group)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	GtkWidget *last;
	GtkWidget *curr;
	last = dr_group_cell_new();
	dr_group_cell_set_group(last, group);
	if (!groupcell)
		return last;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	curr = widget;
	while (groupcell->priv->next)
		groupcell = groupcell->priv->next;
	groupcell->priv->next = (DrGroupCell*)last;
	return curr;
}

GtkWidget *dr_group_cell_delete_from(GtkWidget *widget, GtkWidget *container)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	DrGroupCell *curr = (DrGroupCell*)container;
	//gtk_widget_hide(widget);
	if (!groupcell || !container)
		return NULL;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	if (!IS_GROUP_CELL(curr))
		return NULL;
	if (container != widget)
	{
		while (curr->priv->next != groupcell)
			curr = curr->priv->next;
	}
	else
		curr = NULL;
	gtk_widget_hide(groupcell->priv->button_add);
	//g_free(groupcell->priv->button_add);

	gtk_widget_hide(groupcell->priv->button_del);
	//g_free(groupcell->priv->button_del);

	gtk_widget_hide(groupcell->priv->button_name);
	//g_free(groupcell->priv->button_name);


	gtk_widget_hide((GtkWidget*)groupcell);
	if (curr != NULL)
		curr->priv->next = groupcell->priv->next;
	return (GtkWidget*)groupcell->priv->next;
}

