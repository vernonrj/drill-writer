// Functions that manipulate files go here
//#include "drill.h"
#include "d_gtk.h"
int getInParen(FILE *fp, char **opt_r)
{
	/*
	// get a value inside paren
	char c;
	int i = 0;
	char *opt;
	c = getc(fp);
	opt = (char*) malloc(50*sizeof(char));
	if (c != '(')
		return -1;
	while ((c = getc(fp)) != ')' && i < 49)
	{
		// get what's inside the parens
		opt[i] = c;
		i++;
	}
	opt[i] = '\0';
	*/
	return 0;
}

int file_getline(FILE *fp, char **buffer_r)
{
	// get a line from a file
	int def_size = 5;
	int size = def_size;
	char c;
	char cq;
	char *buffer;
	char *oldbuff;
	int index = 0;
	int done = 0;

	buffer = (char*)malloc(size*sizeof(char));
	do
	{
		c = fgetc(fp);
		if (c == EOF || c == '\n')
		{
			done = 1;
			cq = '\0';
		}
		else
			cq = c;
		if (index == size - 1)
		{
			// need to alloc more memory
			buffer[index] = '\0';
			oldbuff = buffer;
			size = size+def_size;
			buffer = (char*)malloc(size*sizeof(char));
			strcpy(buffer, oldbuff);
			free(oldbuff);
		}
		buffer[index] = cq;
		index++;
	} while (done == 0);
	*buffer_r = buffer;
	if (c == EOF)
		return 1;
	else
		return 0;
	return 0;
}




void open_file(void)
{
	int i, j;
	FILE *fp;
	char c;
	char *buffer;
	char *oldbuff;
	char *data;
	char *operation;
	char *name;
	char *info;
	char *perfnum_buffer;
	int perfnum;
	int excode;
	int size;
	int done;
	int index;
	int cnum;
	float x, y;

	struct perf_proto **perfs;
	struct perf_proto *perf;
	struct set_proto *currset;
	struct coord_proto **coords;


	fp = fopen("save_file","r");
	// get the name
	excode = file_getline(fp, &buffer);
	size = strlen(buffer);
	name = (char*)malloc((size+1)*sizeof(char));
	for (i=0; i<size && buffer[i] != '='; i++)
	{
	}
	i++;
	for (i=i; i<size && buffer[i] == ' '; i++)
	{
	}
	index = i;
	for (i=i; i<size; i++)
	{
		name[i-index] = buffer[i];
	}
	name[i-index] = '\0';
	free(buffer);
	// get the info
	excode = file_getline(fp, &buffer);
	size = strlen(buffer);
	info = (char*)malloc((size+1)*sizeof(char));
	for (i=0; i<size && buffer[i] != '='; i++)
	{
	}
	i++;
	for (i=i; i<size && buffer[i] == ' '; i++)
	{
	}
	index = i;
	for (i=i; i<size; i++)
	{
		info[i-index] = buffer[i];
	}
	info[i-index] = '\0';
	free(buffer);
	// get the performer list
	excode = file_getline(fp, &buffer);
	size = strlen(buffer);
	perfnum_buffer = (char*)malloc((size+1)*sizeof(char));
	for (i=0; i<size && buffer[i] != '='; i++)
	{
	}
	i++;
	for (i=i; i<size && buffer[i] == ' '; i++)
	{
	}
	index = i;
	for (i=i; i<size; i++)
	{
		perfnum_buffer[i-index] = buffer[i];
	}
	perfnum_buffer[i-index] = '\0';
	free(buffer);
	perfnum = atoi(perfnum_buffer) + 5;
	// make the show
	show_destroy(&pshow);
	excode = show_construct(&pshow, perfnum);
	//printf("%s\n", perfnum_buffer);
	free(perfnum_buffer);
	// store name
	free(pshow->showname);
	pshow->showname = (char*)malloc((strlen(name)+1)*sizeof(char));
	strcpy(pshow->showname, name);
	free(name);
	// store info
	free(pshow->showinfo);
	pshow->showinfo = (char*)malloc((strlen(info)+1)*sizeof(char));
	strcpy(pshow->showinfo, info);
	free(info);

	// pass whitespace
	done = 0;
	/*
	do {
		excode = file_getline(fp, &buffer);
		if (buffer[0] != '\0')
			done = 1;
		free(buffer);
	} while (done == 0);
	*/

	buffer = (char*)malloc(sizeof(char));
	// get performer info
	perfs = pshow->perfs;
	perf = perfs[0];
	done = 0;
	i = -1;
	do
	{
		/*
		if (i == -1)
			perf = perfs[0];
		else
			perf = perfs[i];
		*/
		free(buffer);
		excode = file_getline(fp, &buffer);
		size = strlen(buffer);
		data = (char*)malloc((size+1)*sizeof(char));
		operation = (char*)malloc((size+1)*sizeof(char));
		strcpy(operation, buffer);
		for (j = 0; j < size && buffer[j] != '='; j++)
		{
			if (buffer[j] == ' ')
				operation[j] = '\0';
		}
		operation[j] = '\0';
		// pass equality
		if (j < size)
			j++;
		// pass whitespace
		for (j = j; j < size && buffer[j] == ' '; j++)
		{
		}
		index = j;
		for (j = index; j < size; j++)
		{
			data[j-index] = buffer[j];
		}
		data[j-index] = '\0';
		if (!strcmp(operation, "name"))
		{
			// name
			i++;
			perf = perfs[i];
			free(perf->name);
			perf->name = (char*)malloc((size+1)*sizeof(char));
			strcpy(perf->name, data);
		}
		else if (!strcmp(operation, "index"))
		{
			// info
			perf->index = atoi(data);
		}
		else if (!strcmp(operation, "symbol"))
		{
			// symbol
			free(perf->symbol);
			perf->symbol = (char*)malloc((size+1)*sizeof(char));
			strcpy(perf->symbol, data);
		}
		else if (!strcmp(operation, "valid"))
		{
			// validity flag
			perf->valid = atoi(data);
		}
		else if (!strcmp(operation, "sets:"))
		{
			// moving to coords. finished with perfs
			done = 1;
		}
		free(data);
		free(operation);
	} while (done == 0);

	currset = pshow->currset;
	do
	{
		free(buffer);
		excode = file_getline(fp, &buffer);
		//printf("buffer = %s\n", buffer);
		size = strlen(buffer);
		data = (char*)malloc((size+1)*sizeof(char));
		operation = (char*)malloc((size+1)*sizeof(char));
		strcpy(operation, buffer);
		for (i = 0; i < size && buffer[i] != '='; i++)
		{
			if (buffer[i] == ' ')
				operation[i] = '\0';
		}
		operation[i] = '\0';
		// pass equal
		if (i < size)
			i++;
		// pass whitespace
		for (i = i; i < size && buffer[i] == ' '; i++)
		{
		}
		// get data
		index = i;
		for (i = index; i < size; i++)
		{
			data[i-index] = buffer[i];
		}
		data[i-index] = '\0';
		// find correct operation
		if (!strcmp(operation, "set"))
		{
			// set
			if (atoi(data) != 0)
			{
				// need a new set
				newset_create(pshow->currset);
				set_next();
				currset = pshow->currset;
			}
		}
		else if (!strcmp(operation, "counts"))
		{
			// counts
			currset->counts = atoi(data);
		}
		else if (!strcmp(operation, "tempo"))
		{
			// tempo
			change_tempo(atoi(data), &pshow->currtempo);
			//printf("ping\n");

		}
		else if (!strcmp(operation, "coords:"))
		{
			// store coords
			coords = currset->coords;
			for (i=0; i<perfnum; i++)
			{
				free(buffer);
				excode = file_getline(fp, &buffer);
				if (buffer[0] == '\0')
					break;
				size = strlen(buffer);
				free(data);
				data = (char*)malloc((size+1)*sizeof(char));
				// TODO: more safety
				for (j=0; j<size && buffer[j] != ':'; j++)
					data[j] = buffer[j];
				data[j] = '\0';
				cnum = atoi(data);
				j++;
				// pass any whitespace
				for (j=j; j<size && buffer[j] == ' '; j++)
				{
				}
				index = j;
				// get first number
				for (j=j; j<size && buffer[j] != ' '; j++)
				{
					data[j-index] = buffer[j];
				}
				data[j-index] = '\0';
				x = atof(data);
				// pass whitespace
				for (j=j; j<size && buffer[j] == ' '; j++)
				{
				}
				index = j;
				// get second number
				for (j=j; j<size; j++)
				{
					data[j-index] = buffer[j];
				}
				data[j-index] = '\0';
				y = atof(data);
				//printf("(x,y) = %i %.2f %.2f\n", cnum, x, y);
				set_coord(coords[i], x, y);
			}
		}
		free(operation);
		free(data);
	} while (excode != 1);
	free(buffer);
	set_first();
	update_tempo();
	fclose(fp);
		
	return;
}




	/*
	// Open a file and use the stuff inside
	FILE *fp;
	char c;
	char *opt;
	fp = fopen("new_save", "r");
	c = getc (fp);
	// allocate
	opt = (char*) malloc(sizeof(char));
	do
	{
		if (c == 'S')
		{
			getInParen(fp, &opt);
		}
	} whi




	fclose(fp);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}
*/

void save_file(GtkWidget *widget)
{
	// save a file
	// sets
	struct set_proto *currset;
	// performers
	struct perf_proto **perfs;
	// coordinates
	struct coord_proto **coords;
	// tempo
	struct tempo_proto *tempo;
	// file pointer
	FILE *fp;
	// loop var
	int i, j;
	int set_num = setnum;
	int done;

	// open file
	set_first();
	currset = pshow->firstset;
	fp = fopen("save_file","w");
	fprintf(fp, "name = %s\n", pshow->showname);
	fprintf(fp, "info = %s\n", pshow->showinfo);
	fprintf(fp, "perfnum = %i\n", pshow->perfnum);
	fprintf(fp, "\n");
	// store performers
	perfs = pshow->perfs;
	for (i=0; i<pshow->perfnum; i++)
	{
		fprintf(fp, "name = %s\n", perfs[i]->name);
		fprintf(fp, "index = %i\n", perfs[i]->index);
		fprintf(fp, "symbol = %s\n", perfs[i]->symbol);
		fprintf(fp, "valid = %i\n", perfs[i]->valid);
		fprintf(fp, "\n");
	}
	fprintf(fp, "sets:\n\n");
	done = 0;
	do
	{
		// store set info
		coords = currset->coords;

		fprintf(fp, "set = %i\n", setnum);
		fprintf(fp, "counts = %i\n", currset->counts);
		fprintf(fp, "tempo = %i\n", pshow->currtempo->tempo);
		fprintf(fp, "coords:\n");
		for(i=0; i<pshow->perfnum; i++)
		{
			fprintf(fp, "%i: %f %f\n", i, coords[i]->x, coords[i]->y);
		}
		fprintf(fp, "\n");
		set_next();
		update_tempo();
		if (currset->next == NULL)
			done = 1;
		currset = pshow->currset;
	} while (done == 0);
	fclose(fp);
	return;

	/*
	FILE *fp;
	int i, j;	// loop vars
	char string_x[20];
	char string_y[20];
	int len_x, len_y;
	fp = fopen("save_file","w");

	fprintf(fp,"I %i %i %i\n", setnum, set_tot, perfnum);
	for (i=0; i<set_tot; i++)
	{
		//len_x = sprintf(string_x, "%g", perf[i][j][0]);
		//len_y = sprintf(string_y, "%g", perf[i][j][1]);
		fprintf(fp, "S %i %i\n", i, counts[i]); // set number, count structure
		for(j=0; j<perfnum; j++)
		{
			fprintf(fp, "%i %g %g\n", j, perf[i][j][0], perf[i][j][1]);
			//printf("%f\n", perf[i][j][0]);
		}
	}


	fclose(fp);
	*/
}
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

int wrap_load_dep(GtkWidget *widget)
{
	// a wrapper for the deprecated load function
	// interface with the old static array type
	open_file();
	return 0;
	int i, j;
	float x, y;
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
			set_coord_valid(currset->coords, j, x, y);
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
