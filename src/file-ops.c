// Functions that manipulate files go here
//#include "drill.h"
#include "drill.h"


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
	buffer[index] = '\0';
	*buffer_r = buffer;
	if (c == EOF)
		return 1;
	else
		return 0;
	return 0;
}


int file_getValidLine(FILE *fp, char **buffer_r)
{
	// get a valid line (not a blank-line)
	// 	from a file
	int excode;
	char *buffer;
	int done;
	do
	{
		excode = file_getline(fp, buffer_r);
		buffer = *buffer_r;
		if (excode == 1)
			done = 1;
		else if (buffer[0] == '\0')
		{
			// empty line
			done = 0;
			free(buffer);
		}
		else
		{
			// finished
			done = 1;
		}
	} while (done == 0);
	return excode;
}



int file_linetoOps(char *buffer, char **op_r, char stop)
{
	// convert a line to an operation using the stopchar
	//char *buffer;
	char *op;
	int size;
	int i;
	int index = 0;
	bool match;
	bool foundOp = 0;
	printf("line = %s\n", buffer);
	size = strlen(buffer);
	op = (char*)malloc((size+2)*sizeof(char));
	if (op == NULL)
		return -1;
	strncpy(op, buffer, size);
	match = 0;
	for (i=0; i<size; i++)
	{
		// find the operation using stopchar
		if (op[i] == ' ' && !foundOp)
		{
			// end of operation
			op[i] = '\0';
			foundOp = 1;
		}
		if (buffer[i] == stop)
		{
			// stop char found
			match = 1;
			index = i;
		}
		else if (i-1 == index && buffer[i] == ' ')
		{
			// remove spaces
			index++;
		}
		if (match == true)
			buffer[i-index] = buffer[i];
	}
	buffer[i-index] = '\0';
	printf("operat = %s\n", op);
	printf("buffer = %s\n", buffer);
	//*buffer_r = buffer;
	*op_r = op;
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
	int *valid;
	char *nwdata;
	char *op;

	struct perf_proto **perfs;
	struct perf_proto *perf;
	struct set_proto *currset;
	struct coord_proto **coords;


	fp = fopen("save_file","r");
	// get the name
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	name = (char*)malloc((size+2)*sizeof(char));
	strncpy(name, buffer, size+1);
	excode = file_linetoOps(name, &op, '=');
	free(op);
	free(buffer);

	// get info
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	info = (char*)malloc((size+2)*sizeof(char));
	strncpy(info, buffer, size+1);
	excode = file_linetoOps(info, &op, '=');
	free(op);
	free(buffer);

	// get perfnum
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	perfnum_buffer = (char*)malloc((size+2)*sizeof(char));
	strncpy(perfnum_buffer, buffer, size+1);
	excode = file_linetoOps(perfnum_buffer, &op, '=');
	free(op);
	free(buffer);
	perfnum = atoi(perfnum_buffer);
	/*
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
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);

	nwdata = (char*)malloc((size+2)*sizeof(char));
	op = (char*)malloc((size+2)*sizeof(char));
	strncpy(nwdata, buffer, size+1);
	printf("first buffer : %s\n", nwdata);
	file_linetoOps(nwdata, &op, '=');
	free(nwdata);
	free(op);

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
	perfnum = atoi(perfnum_buffer);
	*/
	// make the show
	show_destroy(&pshow);
	excode = show_construct(&pshow, perfnum);
	printf("perfnum = %i\n", perfnum);
	valid = (int*)malloc(perfnum*sizeof(int));
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
		if (!strcmp(operation, "index"))
		{
			// info
			perf->index = atoi(data);
			i++;
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
			valid[i] = atoi(data);
		}
		else if (!strcmp(operation, "sets:"))
		{
			// moving to coords. finished with perfs
			done = 1;
		}
		free(data);
		free(operation);
	} while (done == 0);
	for (i=i+1; i<perfnum; i++)
		valid[i] = 0;

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
			i = 0;
			//for (i=0; i<perfnum; i++)
			do
			{
				free(buffer);
				excode = file_getline(fp, &buffer);
				if (buffer[0] == '\0')
					break;
				else if (!strcmp(buffer, "done"))
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
				if (valid[i] != -1)
					set_coord(pshow, cnum, x, y);
				i++;
			} while (i < perfnum);
		}
		free(operation);
		free(data);
	} while (excode != 1);
	free(buffer);
	set_first();
	update_tempo();
	fclose(fp);

	// update valid flags
	printf("perfnum = %i\n", perfnum);
	for (i=0; i<perfnum; i++)
		perfs[i]->valid = valid[i];

	undo_destroy(&pshow->undobr, pshow);
		
	return;
}




void save_file(void)
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
		if (perfs[i]->valid != -1 && perfs[i]->valid != 0)
		{
			fprintf(fp, "index = %i\n", perfs[i]->index);
			fprintf(fp, "symbol = %s\n", perfs[i]->symbol);
			fprintf(fp, "valid = %i\n", perfs[i]->valid);
			fprintf(fp, "\n");
		}
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
			if (perfs[i]->valid)
				fprintf(fp, "%i: %f %f\n", i, coords[i]->x, coords[i]->y);
		}
		fprintf(fp, "done\n");
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

