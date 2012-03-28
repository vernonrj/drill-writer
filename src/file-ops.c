// Functions that manipulate files go here
#include "drill.h"
void open_file(void)
{
	/*
	// Open a file and use the stuff inside
	FILE *fp;
	int i, j;
	int work;
	char buffer[20];
	double dbuff;
	char c;
	int wset;
	int wperf;
	fp = fopen("save_file", "r");
	c = getc (fp);
	i = 0;
	if (c == 'I')
	{	// Show Defs
		c = getc (fp);
		c = getc (fp);
		while (c != ' ' && c != '\n')
		{	// Get set number
			buffer[i] = c;
			c = getc (fp);
			i++;
		}
		if (c == '\n')
			gtk_main_quit();
		buffer[i] = '\0';
		setnum = atoi(buffer);
		c = getc(fp);
		i=0;
		while (c != ' ')
		{	// get set total
			buffer[i] = c;
			c = getc (fp);
			i++;
		}
		buffer[i] = '\0';
		set_tot = atoi(buffer);

		c = getc(fp);
		i = 0;
		while (c != '\n')
		{
			// get performer total
			buffer[i] = c;
			c = getc (fp);
			i++;
		}
		buffer[i] = '\0';
		perfnum = atoi(buffer);

	}
	c = getc(fp);
	//printf("Hit 1\n");


	while (c != EOF)
	{
		// have to figure out how to advance to next set
		if (c == 'S')
		{
			c = getc(fp);
			c = getc (fp);
			i=0;
			while (c != ' ')
			{
				buffer[i] = c;
				c = getc(fp);
				i++;
			}
			buffer[i] = '\0';
			//printf("buffer = %s\n\n", buffer);
			work = atoi(buffer);
			wset = work;
			c = getc(fp);
			i=0;
			//printf("Hit 2\n");
			while (c != '\n')
			{
				buffer[i] = c;
				c = getc(fp);
				i++;
			}
			buffer[i] = '\0';
			work = atoi(buffer);
			counts[wset] = work;
		}
		//printf("Hit 3\n");

		c = getc(fp);
		j=0;
		while (c != 'S' && c != EOF)
		{

			i=0;
			while (c != ' ' && c != '\n' && c != EOF)
			{
				buffer[i] = c;
				i++;
				c = getc (fp);
			}
			buffer[i] = '\0';
			dbuff = atol (buffer);
			work = atoi (buffer);
			if (j==0)
			{
				//printf("\n");
				wperf = work;
			}
			if (j==1)
				perf[wset][wperf][0] = dbuff;
			if (j==2)
				perf[wset][wperf][1] = dbuff;
			//printf("perf %i at set %i: X = %g\tY = %g\n", wperf, wset, perf[wset][wperf][0], perf[wset][wperf][1]);
			j++;
			if (j==3)
				j=0;
			c = getc (fp);
		}
		if (c != EOF && c != 'S')
			c = getc(fp);
	}

	fclose(fp);
	setnum = 0;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	*/
}

void save_file(GtkWidget *widget)
{
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

void absolute_dot (void)
{
	/*
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
	printf("string = %s\n", string);
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
	printf("x = %g\n", xdot);
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
	*/
}
void relative_dot (void)
{
	/*
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

	*/
}
void func_relative(void)
{
	/*
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
	*/
}

void show_gen(GtkWidget *widget)
{
	/*
	int i, j;
	double intervalx, intervaly;
	fp = fopen("new_save", "r");
	c = getc(fp);
	do	// while c != EOF
	{
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
				tempo = atoi(string);
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
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	*/
}
