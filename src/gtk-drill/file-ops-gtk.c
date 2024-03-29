#include "drillwriter-gtk.h"

/*
void save_file_gtk(GtkWidget *widget)
{
	save_file();
}
*/


/*
double perf[50][50][3];
int perfnum;
int set_tot;
FILE *fp;
char c;
char comm[20];
char string[40];
char op;
double rdot, xdot, ydot, xadot[50], yadot[50];
int side, yardline;
char frontback, hash[3];
int array[100];
int wset, wcount, wperf, work;
double x, y;
int place;
int counts[50];
*/

int new_file_gtk(GtkWidget *widget)
{
	// make a new file
	int excode;
	show_destroy(&pshow);
	excode = show_construct(&pshow, 5);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	if (excode == -1)
		return -1;
	return 0;
}


int open_file_gtk(GtkWidget *widget)
{
	GtkWidget *dialog;
	char *filename;
	int excode = 0;
	dialog = gtk_file_chooser_dialog_new("Open File",
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
	if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		printf("file = %s\n", filename);
		excode = open_file(filename);
		if (excode == 0)
		{
			// worked. Save filename for savefile use
			pstate.filename = filename;
		}
		else
			g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return excode;
}


int save_file_gtk(GtkWidget *widget)
{
	// save a file
	int excode;
	if (pstate.filename)
	{
		// should have a valid save file.
		// use that
		excode = save_file(pstate.filename);
	}
	else
	{
		// no save file found. do save as dialog
		excode = save_file_as_gtk(widget);
	}
	return excode;
}



int save_file_as_gtk(GtkWidget *widget)
{
	// save a file as a specified filename
	int excode = 0;
	char *filename;
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Save File",
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(dialog), TRUE);
	if (!pstate.filename)
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Untitled");
	else
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), pstate.filename);
	if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
		excode = save_file(filename);
		if (excode == 0)
		{
			// worked. Save new filename for savefile use
			pstate.filename = filename;
		}
		else
			g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return excode;
}

	

int wrap_load_dep(GtkWidget *widget)
{
	// a wrapper for the deprecated load function
	// interface with the old static array type
	open_file("save_file");
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return 0;
}
	/*
	int i, j;
	double x, y;
	struct set_proto *currset;
	struct set_proto *prevset;
	struct tempo_proto *stempo;
	show_gen(&stempo);
	show_construct(&pshow, perfnum);
	free(pshow->currtempo);
	pshow->currtempo = stempo;
	for (i=0; i<set_tot; i++)
	{
		if (i != 0)
		{
			// not the first set
			newset_create(currset);
			prevset = currset;
			currset = currset->next;
			currset->counts = counts[i];
		}
		else
		{
			// first set
			currset = pshow->firstset;
			prevset = 0;
			currset->counts = 0;
		}
		for (j=0; j<perfnum; j++)
		{
			x = perf[i][j][0];
			y = perf[i][j][1];
			set_coord(pshow, j, x, y);
		}
	}
	pshow->currset = pshow->firstset;
	pshow->prevset = 0;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}
	
void absolute_dot (void)
{
	int i, j;
	i = 0;
	do
	{	// get x-coord
		string[i] = c;
		i++;
		c = getc(fp);
	} while (c != '.');
	c = getc(fp);
	string[i] = '\0';
	//printf("string = %s\n", string);
	if (!place)
	{
		xdot = atol(string);
	}
	else
	{
		ydot = atol(string);
	}
	i=0;
	if (place)
		frontback = c;	// set front/back
	else
	{
		if (c == 's')
			c = getc(fp); // go past "s"
		// get side
		string[0] = c;
		string[1] = '\0';
		side = atoi(string);
	}
	c = getc(fp);
	c = getc(fp);
	i=0;
	do
	{	// get yardline
		string[i] = c;
		i++;
		c = getc(fp);
	} while (c != ',' && c != ' ' && c != ')');
	string[i] = '\0';
	if (!place)
	{
		yardline = atoi(string);
	}
	else
	{
		strcpy(hash, string);
	}
	if (side == 1)
		x=xdot+yardline/5*8;
	else if (side == 2)
		x=50/5*8-xdot+(50-yardline)/5*8;
	//printf("x = %g\n", xdot);
	// y conversion
	if (!strcmp(hash, "fs"))
		y = 85;
	else if (!strcmp(hash, "fh"))
		y = 53;
	else if (!strcmp(hash, "bh"))
		y = 32;
	else if (!strcmp(hash, "bs"))
		y = 0;
	if (frontback == 'f')
		y = y+ydot;
	else if (frontback == 'b')
		y = y-ydot;

	//perf[wset][wperf][0] = x;
	//perf[wset][wperf][1] = y;
	place++;
	while (c == ',' || c == ' ' || c == '\n')
		c = getc(fp);
}

void relative_dot (void)
{
	int i, j;
	i = 0;
	c = getc(fp);
	do
	{	// related dot
		//printf("rel c = %c\n", c);
		string[i] = c;
		i++;
		c = getc(fp);
	} while (c != '+' && c != '-' && c != ',' && c != ' ' && c != ')' && c != '\n');
	string[i] = '\0';
	//printf("perf=%s c=%c\n", string, c);
	work = atoi(string);

	if (c == '\n' || c == ',')
		op = '=';
	else
	{
		op = c;
		c = getc(fp);
	}
	i=0;
	while (c != ',' && c != '\n' && c != ')')
	{	// get number
		string[i] = c;
		i++;
		c = getc(fp);
	} 
	string[i] = '\0';
	//printf("string = %s perf=%i %c other perf=%i\n", string, wperf, op, work);
	if (!place)
	{
		x = perf[wset][work][0];
		if (op == '+')
			x = x+atol(string);
		else if (op == '-')
			x = x-atol(string);
		//perf[wset][wperf][0] = x;
		//printf("x = %g\n", x);
	}
	else
	{
		y = perf[wset][work][1];
		if (op == '+')
			y = y+atol(string);
		else if (op == '-')
			y = y-atol(string);
		//perf[wset][wperf][1] = y;
	}
	
	place++;
	while (c == ',' || c == ' ' || c == ';')
	{
		c = getc(fp);
		//printf("c = %c\n", c);
	}

}

void func_relative(void)
{
	int i;
	while (c != '+' && c != '-' && c != ' ' && c != ')')
		c = getc(fp);
	i=0;
	if (c == '+')
		op='+';
	else if (c == '-')
		op='-';
	else
		op='=';
	while (c != ' ' && c != ',' && c != ')')
	{
		string[i] = c;
		i++;
		c = getc(fp);
	}
	string[i] = '\0';
	//printf("number=%s\n", string);
	if (!place)
	{
		if (op == '+')
			x = xadot[0]+atol(string);
		else if (op == '-')
			x = xadot[0]-atol(string);
	}
	else
	{
		if (op == '+')
			y = yadot[0]+atol(string);
		else if (op == '-')
			y = yadot[0]-atol(string);
	}
	place++;
	while (c == ',' || c == ' ' || c == ';')
		c = getc(fp);
}

void show_gen(struct tempo_proto **stempo_r)
{
	int i, j;
	//int tempo;
	double intervalx, intervaly;
	struct tempo_proto *stempo;
	int tempo;
	tempo_construct(&stempo, 0);
	fp = fopen("new_save", "r");
	c = getc(fp);
	do	// while c != EOF
	{
		setnum = wset;
		//printf("%c\n\n", c);
		if (c == 'S' || c == 'D') 
		{	// set/define something
			c = getc(fp);
			c = getc(fp);
			i = 0;
			do	// go until we hit a parentheses
			{
				comm[i] = c;
				i++;
				c = getc(fp);
			} while (c!=')');
			comm[i] = '\0';
			//printf("command=%s\n", comm);
			c = getc(fp);
			if (c==' ' || c=='=')
				c=getc(fp);
			if (c=='(' || c==' ')
				c=getc(fp);
			i=0;
			do
			{	// go until we hit a semicolon
				string[i] = c;
				i++;
				c = getc(fp);
			} while (c!=';');
			string[i] = '\0';
			//printf("value=%s\n", string);
			if (!(strcmp(comm, "set")))
			{
				wset=atoi(string);
				for (i=0; i<perfnum; i++)
				{
					perf[wset][i][0] = perf[wset-1][i][0];
					perf[wset][i][1] = perf[wset-1][i][1];
				}
			}
			else if (!(strcmp(comm, "counts")))
				counts[wset] = atoi(string);
			else if (!(strcmp(comm, "perf")))
				perfnum=atoi(string);
			else if (!(strcmp(comm, "tempo")))
			{
				tempo = atoi(string);
				change_tempo(tempo, &stempo);
			}
		}
		else if (c=='G')
		{
			// Group
			c = getc(fp);
			while (c == '(' || c == ' ')
				c = getc(fp);
			i=0;
			// Grab type of group function
			while (c != '[' && c != ' ' && c != '=')
			{
				string[i] = c;
				i++;
				c = getc(fp);
			}
			string[i] = '\0';
			if (!strcmp(string, "line"))
			{
				c = getc(fp);
				c = getc(fp); // past 'P'
				j = 0;
				while (c != ']')
				{	// get all performers
					i = 0;
					while (c == ',' || c == ' ' || c == 'P')
						c = getc(fp);
					while (c != ',' && c != ']')
					{
						string[i] = c;
						i++;
						c = getc(fp);
					}
					string[i] = '\0';
					//printf("perf value = %s\n", string);
					array[j] = atoi(string);
					j++;
				}
				// move past junk
				while (c == ' ' || c == ',' || c =='(' || c == ']')
					c = getc(fp);
				place=0;
				while (place < 2)
				{
					if (c == 'P')
						relative_dot();
					else
						absolute_dot();
				}
				xadot[0] = x;
				yadot[0] = y;
				//printf("xadot = %g\n", xadot[0]);
				
				// move past junk
				while (c == ' ' || c == ',' || c == '(' || c == ')')
					c = getc(fp);
				place=0;
				while (place < 2)
				{
					if (c == 'P')
						relative_dot();
					else if (c == 'x' || c == 'y')
						func_relative();
					else
						absolute_dot();
				}
				intervalx=(x-xadot[0])/(j-1);
				intervaly=(y-yadot[0])/(j-1);
				for (i=0; i<j; i++)
				{
					perf[wset][array[i]][0] = intervalx*i+xadot[0];
					perf[wset][array[i]][1] = intervaly*i+yadot[0];
				}
				//printf("dot is %g %g\n", xadot[0], yadot[0]);
				
				
			}
		}

		else if (c=='P')
		{
			i=0;
			c = getc(fp);
			do
			{
				comm[i] = c;
				i++;
				c = getc(fp);
			} while (c != '=' && c != ' ');
			comm[i] = '\0';
			wperf = atoi(comm);
			while (c == '=' || c == ' ' || c == '(')
				c = getc(fp);
			place=0;
			while (place < 2)
			{
				if (c == 'P')
					relative_dot();
				else
					absolute_dot();
			}
			perf[wset][wperf][0] = x;
			perf[wset][wperf][1] = y;
					
			// x conversion
			//printf("set = %i, perf=%i, x=%g, y=%g\n", wset, wperf, perf[wset][wperf][0], perf[wset][wperf][1]);
		}
		else if (c == '#')
		{
			while (c!= '\n' && c != EOF)
				c = getc(fp);
		}
		if (c != EOF)
			c = getc(fp);
	} while (c != EOF);
	set_tot = wset+1;
	fclose (fp);
	*stempo_r = stempo;
	setnum = 0;
}
*/
