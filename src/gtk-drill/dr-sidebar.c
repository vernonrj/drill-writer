#include "d_gtk.h"
#include "dr-sidebar.h"
#include "dr-sidebar-perfs.h"
#include "dr-sidebar-sets.h"
//#include "dr-sidebar-groups.h"

static void dr_sidebar_class_init(DrSidebarClass *class);
static void dr_sidebar_init(DrSidebar *sidebar);


struct _DrSidebarPrivate 
{
	int shown;
	GtkWidget *combo_box;
	GtkWidget *notebook;
	GtkWidget *sidebarsets;
	GtkWidget *sidebarperfs;
	GtkWidget *sidebargroups;
};

G_DEFINE_TYPE (DrSidebar, dr_sidebar, GTK_TYPE_VBOX)

#define DR_SIDEBAR_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SIDEBAR_TYPE, DrSidebarPrivate))

static void dr_sidebar_class_init(DrSidebarClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSidebarPrivate));
}

static void dr_sidebar_init(DrSidebar *lsidebar)
{
	//g_return_if_fail(IS_SIDEBAR(lsidebar));
	GtkWidget *combo_box;
	GtkWidget *notebook;
	//GtkWidget *sidebarsets;
	static const char *sidebar_opts[] = {
		"Set", "Performer", "Groups"
	};

	lsidebar->priv = DR_SIDEBAR_GET_PRIVATE(lsidebar);
	lsidebar->priv->shown = 1;

	combo_box = create_combo_box(sidebar_opts);
	gtk_box_pack_start(GTK_BOX(lsidebar), combo_box, FALSE, FALSE, 0);
	g_signal_connect(combo_box, "changed",
			G_CALLBACK(dr_sidebar_changed), lsidebar);
	gtk_widget_show(combo_box);
	lsidebar->priv->combo_box = combo_box;

	// sidebar notebook
	notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
	gtk_box_pack_start(GTK_BOX(lsidebar), notebook, TRUE, TRUE, 0);
	gtk_widget_show(notebook);
	lsidebar->priv->notebook = notebook;

	lsidebar->priv->sidebarsets = dr_sidebar_sets_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lsidebar->priv->sidebarsets, NULL);

	lsidebar->priv->sidebarperfs = dr_sidebar_perfs_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lsidebar->priv->sidebarperfs, NULL);

	lsidebar->priv->sidebargroups = dr_sidebar_groups_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lsidebar->priv->sidebargroups, NULL);

	//lsidebar->priv->sidebarsets = sidebarsets;
}

GtkWidget *dr_sidebar_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_get_type(), NULL));
}

GtkWidget *create_combo_box (const char **strings)
{
	GtkWidget *combo_box;
	const char **str;

	combo_box = gtk_combo_box_text_new ();

	for (str = strings; *str; str++)
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), *str);

	gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);

	return combo_box;
}

void dr_sidebar_changed(GtkComboBox *widget, DrSidebar *lsidebar)
{
	int sel = gtk_combo_box_get_active(widget);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(lsidebar->priv->notebook), sel);
	return;
}

void dr_sidebar_update(DrSidebar *lsidebar)
{
	g_return_if_fail(IS_SIDEBAR(lsidebar));
	dr_sidebar_sets_update(lsidebar->priv->sidebarsets);
	dr_sidebar_perfs_update(lsidebar->priv->sidebarperfs);
	return;
}

void dr_sidebar_toggle(GtkWidget *widget)
{
	DrSidebar *lsidebar;
	lsidebar = (DrSidebar*)sidebar;
	if (lsidebar->priv->shown)
	{
		gtk_widget_hide(sidebar);
		lsidebar->priv->shown = 0;
	}
	else
	{
		gtk_widget_show(sidebar);
		lsidebar->priv->shown = 1;
	}
	return;
}

