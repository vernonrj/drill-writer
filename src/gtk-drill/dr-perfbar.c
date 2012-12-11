#include "drillwriter-gtk.h"
#include "dr-setbar.h"
#include "dr-perfbar.h"
#include "../dr_fieldrel.h"
#include "../coords.h"


static void dr_perfbar_class_init(DrPerfbarClass *klass);
static void dr_perfbar_init(DrPerfbar *perfbar);


struct _DrPerfbarPrivate {
	GtkWidget *entry_perf;
	// steps from yardline entry
	GtkWidget *ssStepEntry;		
	// inside or outside yardline toggle button
	GtkWidget *ssYdRelButton;	
	// side 1 or side 2 toggle button
	GtkWidget *ssSide;		
	// Yardline button
	GtkWidget *ssYdLine;		

	// Steps from hash/sideline entry
	GtkWidget *fbStepEntry;
	// inside or outside hash/sideline toggle button
	GtkWidget *fbHashRelButton;
	// Front/Back toggle button
	GtkWidget *fbToggleButton;
	// Hash/Sideline
	GtkWidget *HashSideButton;

};

G_DEFINE_TYPE (DrPerfbar, dr_perfbar, GTK_TYPE_HBOX)

#define DR_PERFBAR_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_PERFBAR_TYPE, DrPerfbarPrivate))

static void dr_perfbar_class_init(DrPerfbarClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrPerfbarPrivate));

}

static void dr_perfbar_init(DrPerfbar *perfbar)
{
	GtkWidget *entry_perf;
	GtkWidget *ssStepEntry;
	GtkWidget *ssYdRelButton;
	GtkWidget *ssYdLine;
	GtkWidget *ssSide;
	GtkWidget *fbStepEntry;
	GtkWidget *fbHashRelButton;
	GtkWidget *fbToggleButton;
	GtkWidget *HashSideButton;

	GtkWidget *frame;
	GtkWidget *perfSSbox;
	GtkWidget *perfFBbox;
	GtkWidget *perfEnbox;
	GtkWidget *separator;
	char perf_buf[5];
	//char perf_buf_x[5];
	//char perf_buf_y[5];
	char ss_buf[20];
	char fb_buf[20];

	perfbar->priv = DR_PERFBAR_GET_PRIVATE(perfbar);

	// Performer entry box
	// put it in a frame
	frame = gtk_frame_new("performer");
	//perfEnbox = gtk_hbox_new(FALSE, 5);
	perfEnbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfEnbox);
	gtk_box_pack_start(GTK_BOX(perfbar), frame, FALSE, TRUE, 5);

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	//g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_perf, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (perfEnbox), entry_perf, FALSE, TRUE, 5);

	//separator = gtk_vseparator_new();
	separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start(GTK_BOX(perfbar), separator, FALSE, TRUE, 0);
	gtk_widget_show(separator);


	// Side-to-Side
	// Field Step entry
	frame = gtk_frame_new("side-to-side");
	//perfSSbox = gtk_hbox_new(FALSE, 5);
	perfSSbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfSSbox);
	gtk_box_pack_start(GTK_BOX(perfbar), frame, FALSE, TRUE, 5);
	sprintf(ss_buf, "%i", 0);
	ssStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ssStepEntry), 50);
	g_signal_connect(ssStepEntry, "activate",
		       	G_CALLBACK(coords_change_ss_entry_gtk), ssStepEntry);
	gtk_entry_set_text(GTK_ENTRY(ssStepEntry), ss_buf);
	//tmp_pos = GTK_ENTRY(ssStepEntry)->text_length;
	//gtk_editable_select_region(GTK_EDITABLE(ssStepEntry), 0, GTK_ENTRY(ssStepEntry)->text_length);
	gtk_editable_select_region(GTK_EDITABLE(ssStepEntry), 0, gtk_entry_get_text_length(GTK_ENTRY(ssStepEntry)));
	gtk_entry_set_alignment(GTK_ENTRY(ssStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(ssStepEntry), 5);
	//gtk_box_pack_start(GTK_BOX(perfbar), sidesideBtns.ssStepEntry, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(perfSSbox), ssStepEntry, FALSE, TRUE, 0);

	// yardline relation
	ssYdRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(ssYdRelButton,
		       	"clicked", G_CALLBACK(coords_toggle_ssYdRel_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), sidesideBtns.ssYdRelButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfSSbox), ssYdRelButton, FALSE, FALSE, 0);
	gtk_widget_show(ssYdRelButton);

	// side of the field
	ssSide = gtk_button_new_with_label ("Side 1");
	g_signal_connect(ssSide,
		       	"clicked", G_CALLBACK(coords_toggle_ssSide_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), sidesideBtns.ssSide, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfSSbox), ssSide, FALSE, FALSE, 0);
	gtk_widget_show(ssSide);

	// yardline
	ssYdLine = gtk_button_new_with_label ("50");
	g_signal_connect(ssYdLine,
		       	"clicked", G_CALLBACK(coords_toggle_ssYdLine_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), sidesideBtns.ssYdLine, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX (perfSSbox), ssYdLine, FALSE, FALSE, 5);
	gtk_widget_show(ssYdLine);


	// another separator
	//separator = gtk_vseparator_new();
	separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start(GTK_BOX(perfbar), separator, FALSE, TRUE, 0);
	gtk_widget_show(separator);


	// Front-To-Back
	// frame
	frame = gtk_frame_new("front-to-back");
	//perfFBbox = gtk_hbox_new(FALSE, 5);
	perfFBbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfFBbox);
	gtk_box_pack_start(GTK_BOX(perfbar), frame, FALSE, TRUE, 5);

	// Field Step entry
	sprintf(fb_buf, "%i", 0);
	fbStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(fbStepEntry), 50);
	g_signal_connect(fbStepEntry, "activate",
		       	G_CALLBACK(coords_change_fb_entry_gtk), fbStepEntry);
	gtk_entry_set_text(GTK_ENTRY(fbStepEntry), fb_buf);
	//tmp_pos = GTK_ENTRY(fbStepEntry)->text_length;
	//gtk_editable_select_region(GTK_EDITABLE(fbStepEntry), 0, GTK_ENTRY(fbStepEntry)->text_length);
	gtk_editable_select_region(GTK_EDITABLE(fbStepEntry), 0, gtk_entry_get_text_length(GTK_ENTRY(fbStepEntry)));
	gtk_entry_set_alignment(GTK_ENTRY(fbStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(fbStepEntry), 5);
	//gtk_box_pack_start(GTK_BOX(perfbar), frbkBtns.fbStepEntry, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(perfFBbox), fbStepEntry, FALSE, TRUE, 0);

	// Hash relation button
	fbHashRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(fbHashRelButton,
		       	"clicked", G_CALLBACK(coords_toggle_fbHashRel_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), frbkBtns.fbHashRelButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), fbHashRelButton, FALSE, FALSE, 0);
	gtk_widget_show(fbHashRelButton);

	// Front/Back toggle button
	fbToggleButton = gtk_button_new_with_label ("Front");
	g_signal_connect(fbToggleButton,
		       	"clicked", G_CALLBACK(coords_toggle_fbFrontBack_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), frbkBtns.fbToggleButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), fbToggleButton, FALSE, FALSE, 0);
	gtk_widget_show(fbToggleButton);

	// Hash/Sideline toggle button
	HashSideButton = gtk_button_new_with_label ("Sideline");
	g_signal_connect(HashSideButton,
		       	"clicked", G_CALLBACK(coords_toggle_HashSide_gtk), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbar), frbkBtns.HashSideButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), HashSideButton, FALSE, FALSE, 0);
	gtk_widget_show(HashSideButton);

	perfbar->priv->entry_perf = entry_perf;
	perfbar->priv->ssStepEntry = ssStepEntry;
	perfbar->priv->ssYdRelButton = ssYdRelButton;
	perfbar->priv->ssYdLine = ssYdLine;
	perfbar->priv->ssSide = ssSide;
	perfbar->priv->fbStepEntry = fbStepEntry;
	perfbar->priv->fbHashRelButton = fbHashRelButton;
	perfbar->priv->fbToggleButton = fbToggleButton;
	perfbar->priv->HashSideButton = HashSideButton;


	gtk_widget_show(perfEnbox);
	gtk_widget_show(perfSSbox);
	gtk_widget_show(perfFBbox);
	//gtk_widget_show(perfbar);
}

GtkWidget *dr_perfbar_new(void)
{
	return GTK_WIDGET(g_object_new(dr_perfbar_get_type(), NULL));
}

int update_entries(void)
{
	DrPerfbar *aperfbar;
	char perf_buf[5];
	char perf_buf_x[5];
	char perf_buf_y[5];
	// side-to-side entries/buttons
	char ss_buf[20];
	char ss_ydrel[10];
	char ss_siderel[8];
	char ss_ydline[3];
	// front-to-back entries/buttons
	double ssStep;
	double fbStep;
	int yardRel;
	int fieldSide;
	double cx, cy;
	gchar *fb_buf;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;

	aperfbar = (DrPerfbar*)perfbar;

	// Update all the entries
	coords_retrieve(pstate.center, &cx, &cy);
	//cx = pstate.center->x;
	//cy = pstate.center->y;
	ssStep = fieldrel_get_side_to_side(&cx, &cy);
	sprintf(ss_buf, "%.2f", ssStep);
	yardRel = fieldrel_check_is_inside_yard(&cx, &cy, &fieldSide);
	if (yardRel == -1)
	{
		// outside
		sprintf(ss_ydrel, "outside");
	}
	else if (yardRel == 1)
	{
		// inside
		sprintf(ss_ydrel, "inside");
	}
	else
	{
		// on
		sprintf(ss_ydrel, "on");
	}
	sprintf(ss_siderel, "side %i", fieldSide);
	sprintf(ss_ydline, "%i", fieldrel_get_yardline_number(&cx, &cy));
	fbStep = fieldrel_get_front_to_back(&cx, &cy, &fb_hashrel, &fb_frontback, &fb_hashside);
	fb_buf = g_strdup_printf("%.2f", fbStep);
	entry_upifCh(aperfbar->priv->ssStepEntry, ss_buf);
	// change yard relation button
	button_upifCh(aperfbar->priv->ssYdRelButton, ss_ydrel);
	button_upifCh(aperfbar->priv->ssSide, ss_siderel);
	button_upifCh(aperfbar->priv->ssYdLine, ss_ydline);
	entry_upifCh(aperfbar->priv->fbStepEntry, fb_buf);
	button_upifCh(aperfbar->priv->fbHashRelButton, fb_hashrel);
	button_upifCh(aperfbar->priv->fbToggleButton, fb_frontback);
	button_upifCh(aperfbar->priv->HashSideButton, fb_hashside);
	g_free(fb_buf);
	g_free(fb_hashrel);
	g_free(fb_frontback);
	g_free(fb_hashside);

	dr_setbar_update_entries(setbar);
	
	// Convert all numbers into strings first
	// side-to-side relations
	sprintf(perf_buf, "%i", perf_cur);
	//retr_coord(pshow->sets->currset->coords[perf_cur], &x, &y);
	//sprintf(perf_buf_x, "%g", pstate.center->x);
	//sprintf(perf_buf_y, "%g", pstate.center->y);
	sprintf(perf_buf_x, "%g", cx);
	sprintf(perf_buf_y, "%g", cy);
	// Now Update entries with new data
	//gtk_entry_set_text(GTK_ENTRY (entry_sets), set_buf);
	//gtk_entry_set_text(GTK_ENTRY (entry_tempo), tempo_buf);
	//gtk_entry_set_text(GTK_ENTRY (entry_counts), count_buf);
	gtk_entry_set_text(GTK_ENTRY (aperfbar->priv->entry_perf), perf_buf);
	return 0;
}

