
#ifndef __PERFBAR_H__
#define __PERFBAR_H__

#include <gtk-3.0/gtk/gtk.h>

G_BEGIN_DECLS

#define DR_PERFBAR_TYPE		(dr_perfbar_get_type())
#define DR_PERFBAR(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), DR_PERFBAR_TYPE, DrPerfbar))
#define DR_PERFBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), DR_PERFBAR_TYPE, DrPerfbarClass))
#define IS_PERFBAR(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), DR_PERFBAR_TYPE))
#define IS_PERFBAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), DR_PERFBAR_TYPE))

typedef struct _DrPerfbar DrPerfbar;
typedef struct _DrPerfbarClass DrPerfbarClass;
typedef struct _DrPerfbarPrivate DrPerfbarPrivate;

struct _DrPerfbar {
	GtkHBox base_instance;

	DrPerfbarPrivate *priv;
};

struct _DrPerfbarClass {
	GtkHBoxClass parent_class;
	void (* dr_perfbar) (DrPerfbar *perfbar);
};

GType dr_perfbar_get_type(void);
GtkWidget *dr_perfbar_new(void);

G_END_DECLS
#endif // __PERFBAR_H__

int update_entries(void);

