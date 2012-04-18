// Functions that manipulate files go here
//#include "drill.h"
#include "drill.h"


int file_getline(FILE *fp, char **buffer_r)
{
	// get a line from a file
	// buffer is allocated dynamically to fit the line
	
	// default buffer size
	int def_size = 5;
	// dynamic buffer size
	int size = def_size;
	// stream input
	char c;
	// modified stream output
	char cq;
	// buffer for stream
	char *buffer;
	// buffer for reallocating
	char *oldbuff;
	// index for buffer
	int index = 0;
	// flag indicating finish
	int done = 0;

	// allocate starting buffer
	buffer = (char*)malloc(size*sizeof(char));
	if (!buffer)
		return -1;
	do
	{
		// grab stream to end of line
		// or end of file
		c = fgetc(fp);
		if (c == EOF || c == '\n')
		{
			// hit end of file
			// or end of line
			// Finish reading line
			done = 1;
			cq = '\0';
		}
		else
		{
			// get ready to copy char to buffer
			cq = c;
		}
		if (index == size - 1)
		{
			// Allocate more memory to buffer
			buffer[index] = '\0';
			oldbuff = buffer;
			size = size+def_size;
			buffer = (char*)malloc(size*sizeof(char));
			if (!buffer)
				return -1;
			strcpy(buffer, oldbuff);
			free(oldbuff);
		}
		// store data to buffer
		buffer[index] = cq;
		index++;
	} while (done == 0);
	// cap the end of the buffer (just in case)
	buffer[index] = '\0';
	// passing by reference; pass back
	*buffer_r = buffer;
	// if the end of file is reached, note that in return
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

	// exit code
	int excode;
	// stream buffer
	char *buffer;
	// finished flag
	int done;
	do
	{
		// read until EOF is reached,
		// 	or a non-blank line is found
		excode = file_getline(fp, buffer_r);
		buffer = *buffer_r;
		if (excode == 1)
			done = 1;
		else if (buffer[0] == '\0')
		{
			// empty line
			// re-run
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

	// loop var
	int i;
	// operation
	char *op;
	// sizing for buffer
	int size;
	// placeholder for buffer
	int index = 0;
	// check to see if match was found
	bool match;
	// check to see if operation was found
	bool foundOp = 0;
	//printf("line = %s\n", buffer);
	size = strlen(buffer);
	op = (char*)malloc((size+2)*sizeof(char));
	if (op == NULL)
		return -1;
	strncpy(op, buffer, size+1);
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
			op[i] = '\0';
			foundOp = 1;
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
	//printf("operat = %s\n", op);
	//printf("buffer = %s\n", buffer);
	//*buffer_r = buffer;
	*op_r = op;
	return 0;
}


int open_file(void)
{
	// open a file (for now, just save_file)
	
	// loop vars
	int i, j;
	// file pointer
	FILE *fp;
	// stream char
	//char c;
	// stream string
	char *buffer;
	// older buffer
	//char *oldbuff;
	// data storage
	char *data;
	// operation storage
	char *operation;
	// name storage
	char *name;
	// info storage
	char *info;
	// performer storage
	char *perfnum_buffer;
	// performer number
	int perfnum;
	// exit code
	int excode;
	// sizing for allocation
	int size;
	// finished flag
	int done;
	// placeholder for char
	int index;
	// coord number
	int cnum;
	// coordinates
	double x, y;
	// validity array
	//int *valid;
	// new data
	//char *nwdata;
	// new operation
	char *op;

	// Structures
	// performer structures
	struct perf_proto **perfs;
	struct perf_proto *perf;
	// Set structure
	struct set_proto *currset;
	//struct set_container *sets;
	// Coord structure
	//struct coord_proto **coords;

	// open file for reading
	fp = fopen("save_file","r");
	// get the name
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	name = (char*)malloc((size+2)*sizeof(char));
	if (!name)
		return -1;
	strncpy(name, buffer, size+1);
	excode = file_linetoOps(name, &op, '=');
	free(op);
	free(buffer);
	// store name
	free(pshow->showname);
	pshow->showname = (char*)malloc((strlen(name)+1)*sizeof(char));
	if (!pshow->showname)
		return -1;
	strcpy(pshow->showname, name);
	free(name);

	// get info
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	info = (char*)malloc((size+2)*sizeof(char));
	if (!info)
		return -1;
	strncpy(info, buffer, size+1);
	excode = file_linetoOps(info, &op, '=');
	free(op);
	free(buffer);
	// store info
	free(pshow->showinfo);
	pshow->showinfo = (char*)malloc((strlen(info)+1)*sizeof(char));
	if (!pshow->showinfo)
		return -1;
	strcpy(pshow->showinfo, info);
	free(info);

	// get perfnum
	excode = file_getValidLine(fp, &buffer);
	size = strlen(buffer);
	perfnum_buffer = (char*)malloc((size+2)*sizeof(char));
	if (!perfnum_buffer)
		return -1;
	strncpy(perfnum_buffer, buffer, size+1);
	excode = file_linetoOps(perfnum_buffer, &op, '=');
	free(op);
	free(buffer);
	perfnum = atoi(perfnum_buffer);
	free(perfnum_buffer);

	// make the show
	show_destroy(&pshow);
	excode = show_construct(&pshow, perfnum);
	//printf("perfnum = %i\n", perfnum);
	//valid = (int*)malloc(perfnum*sizeof(int));
	
	// build performers
	buffer = (char*)malloc(sizeof(char));
	if (!buffer)
		return -1;
	// get performer info
	perfs = pshow->perfs;
	perf = perfs[0];
	done = 0;
	i = -1;
	do
	{
		if (i != -1)
		{
			if (perfs[i])
				perf = perfs[i];
			else
				exit(-1);
		}
		// grab performer pieces
		free(buffer);
		// get a valid line
		excode = file_getValidLine(fp, &buffer);
		size = strlen(buffer);
		// get operations at data from line
		data = (char*)malloc((size+2)*sizeof(char));
		if (!data)
			return -1;
		strncpy(data, buffer, size+1);
		excode = file_linetoOps(data, &operation, '=');
		// interpret data based on operation
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
			if (!perf->symbol)
				return -1;
			strcpy(perf->symbol, data);
		}
		else if (!strcmp(operation, "valid"))
		{
			// validity flag
			perf->valid = atoi(data);
			//valid[i] = atoi(data);
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
		perfs[i]->valid = 0;

	// go to first set
	set_first();
	currset = pshow->sets->currset;
	// get set information
	do
	{
		free(buffer);
		excode = file_getValidLine(fp, &buffer);
		//printf("buffer = %s\n", buffer);
		size = strlen(buffer);
		data = (char*)malloc((size+2)*sizeof(char));
		if (!data)
			return -1;
		strncpy(data, buffer, size+1);
		file_linetoOps(data, &operation, '=');
		//printf("op = %s data = %s\n", operation, data);
		// find correct operation
		if (!strcmp(operation, "set"))
		{
			// set
			if (atoi(data) != 0)
			{
				// need a new set
				add_set();
				/*
				newset_create(pshow->sets->currset);
				set_next();
				currset = pshow->sets->currset;
				*/
				currset = pshow->sets->currset;
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
			//printf("changing tempo\n");
			//printf("%i: new tempo = %i\n", pstate.setnum, atoi(data));
			change_tempo(atoi(data), &pshow->currtempo);
			update_tempo();
			//printf("ping\n");

		}
		else if (!strcmp(operation, "coords:"))
		{
			// store coords
			//coords = currset->coords;
			i = 0;
			//for (i=0; i<perfnum; i++)
			do
			{
				free(buffer);
				excode = file_getValidLine(fp, &buffer);
				if (excode == -1)
					exit(-1);
				else if (!strcmp(buffer, "done"))
					break;
				size = strlen(buffer);
				free(data);
				data = (char*)malloc((size+2)*sizeof(char));
				if (excode == -1)
					exit(-1);
				strncpy(data, buffer, size+1);
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
				//printf("valid = %i\n", perfs[i]->valid);
				if (perfs[i]->valid != 0)
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
	//printf("perfnum = %i\n", perfnum);
	/*
	for (i=0; i<perfnum; i++)
		perfs[i]->valid = valid[i];
		*/

	undo_destroy(&pshow->undobr, pshow);
		
	return 0;
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
	//struct tempo_proto *tempo;
	// file pointer
	FILE *fp;
	// loop var
	int i;
	//int set_num = setnum;
	int done;

	// open file
	set_first();
	update_tempo();
	currset = pshow->sets->firstset;
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
		update_tempo();

		fprintf(fp, "set = %i\n", pstate.setnum);
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
		if (currset->next == NULL)
			done = 1;
		currset = pshow->sets->currset;
	} while (done == 0);
	fclose(fp);
	return;
}

