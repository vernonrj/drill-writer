
#ifndef __MEDIABAR_H__
#define __MEDIABAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DR_MEDIABAR_TYPE	(dr_mediabar_get_type())
#define DR_MEDIABAR(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_MEDIABAR_TYPE, DrMediabar))
#define DR_MEDIABAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), DR_MEDIABAR_TYPE, DrMediabarClass))
#define IS_MEDIABAR(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_MEDIABAR_TYPE))
#define IS_MEDIABAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_MEDIABAR_TYPE))

typedef struct _DrMediabar DrMediabar;
typedef struct _DrMediabarClass DrMediabarClass;
//typedef struct _DrMediabarPrivate DrMediabarPrivate;

struct _DrMediabar {
	GtkVBox base_instance;

	//DrMediabarPrivate *priv;
};

struct _DrMediabarClass {
	GtkVBoxClass parent_class;
	void (* dr_mediabar) (DrMediabar *mediabar);
};

GType dr_mediabar_get_type(void);
GtkWidget *dr_mediabar_new(void);

G_END_DECLS
#endif // __MEDIABAR_H__

// media-gtk.c
gboolean play_show (GtkWidget *widget);
void stop_show (GtkWidget *widget);
void queue_show (GtkWidget *widget);
void play_show_from_start (GtkWidget *widget);

