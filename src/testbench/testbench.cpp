#include "tb.hpp"

using namespace std;

extern "C" int foo(void)
{
	mainmenu();
	return 0;
}


int mainmenu(void)
{
	// create testbench mainmenu
	int mysel;
	int excode;

	queue *fifo;

	fifo = new queue;
	fifo->enqueue("Testbench");
	fifo->enqueue("Performers...");
	fifo->enqueue("Selection...");
	fifo->enqueue("Sets...");
	fifo->enqueue("Coords...");
	fifo->enqueue("Undo/Redo...");
	fifo->enqueue("Quit");
	do
	{
		mysel = menu(fifo);

		switch(mysel)
		{
			case 1:		// performers
				excode = tbPerfs();
				break;
			case 2:		// Selection
				excode = tbSelect();
				break;
			case 3:		// sets
				excode = tbSets();
				break;
			case 4:		// Coords
				excode = tbCoords();
				break;
			case 5:		// undo/redo
				excode = tbUnRedo();
				break;
			case 6:		// exit
				excode = 1;
				break;
		} 
	} while (excode == 0);

	delete fifo;

	return 0;
}

int tbPerfs(void)
{
	// testbench for performers
	queue *fifo;
	int mysel;
	int excode;
	
	fifo = new queue;

	fifo->enqueue("Performers");
	fifo->enqueue("Attributes...");
	fifo->enqueue("Add");
	fifo->enqueue("Delete");
	fifo->enqueue("Back");
	do
	{
		mysel = menu(fifo);
		switch(mysel)
		{
			case 1:		// attributes
				excode = tbPerfs_attr();
				break;
			case 2:		// Add
				excode = tbPerfs_add();
				break;
			case 3:		// Delete
				excode = tbPerfs_del();
				break;
			case 4:		// Back
				excode = 1;
				break;
		}
	} while (excode == 0);


	delete fifo;
	return 0;
}

int tbSets(void)
{
	// testbench for sets
	queue *fifo;
	int mysel;
	int excode;
	char *set_buf;
	
	fifo = new queue;

	do
	{
	set_buf = new char [20];
	sprintf(set_buf, "Set (%i)", setnum);
	fifo->enqueue(set_buf);
	delete [] set_buf;
	fifo->enqueue("Attributes...");
	fifo->enqueue("Next");
	fifo->enqueue("Prev");
	fifo->enqueue("First");
	fifo->enqueue("Last");
	fifo->enqueue("Coords...");
	fifo->enqueue("Add");
	fifo->enqueue("Delete");
	fifo->enqueue("Back");
		mysel = menu(fifo);
		switch(mysel)
		{
			case 1:		// attributes
				excode = tbSets_attr();
				break;
			case 2:		// Next
				set_next();
				break;
			case 3:		// Prev
				set_prev();
				break;
			case 4:		// First
				set_first();
				break;
			case 5:		// Last
				set_last();
				break;
			case 6:		// coords
				tbCoords();
				break;
			case 7:		// Add
				excode = tbSets_add();
				break;
			case 8:		// Delete
				excode = tbSets_del();
				break;
			case 9:		// Back
				excode = 1;
				break;
		}
		fifo->discard();
	} while (excode == 0);


	delete fifo;
	return 0;
	return 0;
}

int tbUnRedo(void)
{
	popFromStack(pshow, &pshow->undobr, &pshow->redobr);
	return 0;
}

// performers 
int tbPerfs_attr(void)
{
	return 0;
}
int tbPerfs_add(void)
{
	add_perf();
	return 0;
}
int tbPerfs_del(void)
{
	delete_perf_selected();
	return 0;
}


int tbSets_attr(void)
{
	return 0;
}
int tbCoords(void)
{
	int excode;
	int mysel;
	queue *fifo;
	char *set_buf;

	fifo = new queue;
	set_buf = new char [25];
	sprintf(set_buf, "Coords (Set %i)", setnum);
	fifo->enqueue(set_buf);
	delete [] set_buf;
	fifo->enqueue("View Dot...");
	fifo->enqueue("Select Dots...");
	fifo->enqueue("Change Dot...");
	fifo->enqueue("Back");
	do
	{
		mysel = menu(fifo);
		switch(mysel)
		{
			case 1:		// view dots
				excode = tbCoords_view();
				break;
			case 2:		// select dots
				excode = tbSelect();
				break;
			case 3:		// change dots
				excode = tbCoords_change();
				break;
			case 4:		// back
				excode = 1;
				break;
		}
	} while (excode == 0);
	return 0;
}
int tbCoords_view(void)
{
	return 0;
}
int tbSelect(void)
{
	queue *fifo;
	int excode;
	int mysel;

	fifo = new queue;
	fifo->enqueue("Selection");
	fifo->enqueue("View Selects");
	fifo->enqueue("Add Select");
	fifo->enqueue("Select All");
	fifo->enqueue("Select None");
	fifo->enqueue("Back");

	do
	{
		mysel = menu(fifo);
		switch(mysel)
		{
			case 1:		// view
				excode = tbSelect_view();
				break;
			case 2:		// add
				excode = tbSelect_add();
				break;
			case 3:		// Select all
				// non-testbench
				select_all();
				break;
			case 4:		// Select none
				// non-testbench
				select_discard();
				break;
			case 5:		// back
				excode = 1;
				break;
		}
	} while (excode == 0);
	return 0;
}



int tbSelect_view(void)
{
	select_proto *selects = pshow->select;
	coord_proto **coords = pshow->currset->coords;
	perf_proto **perfs = pshow->perfs;
	int i = 0;

	while (selects)
	{
		if (perfs[i]->valid == 1)
		{
			cout << i << ":\t";
			cout << coords[i]->x << ", ";
			cout << coords[i]->y << " | valid = ";
			cout << perfs[i]->valid << endl;
		}
		else if (perfs[i]->valid != 0)
		{
			cout << "ERR: valid not 0 or 1" << endl;
			cout << "on dot " << i << endl;
		}
		i = i + 1;
		selects = selects->next;
	}
	return 0;
}

int tbSelect_add(void)
{
	queue *fifo;
	char *select;
	int excode;

	select = new char [1];
	fifo = new queue;

	fifo->enqueue("Input dot to select: ");
	fdialog(fifo);

	fifo->dequeue(select);
	excode = atoi(select);
	delete [] select;
	delete fifo;
	select_add(excode);

	return 0;
}


int tbCoords_change(void)
{
	return 0;
}
int tbSets_add(void)
{
	return 0;
}
int tbSets_del(void)
{
	return 0;
}

