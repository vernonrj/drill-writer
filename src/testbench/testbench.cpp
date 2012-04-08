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
			case 2:		// sets
				excode = tbSets();
				break;
			case 3:		// coords
				excode = tbCoords();
				break;
			case 4:		// undo/redo
				excode = tbUnRedo();
				break;
			case 5:		// exit
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
			case 6:		// Add
				excode = tbSets_add();
				break;
			case 7:		// Delete
				excode = tbSets_del();
				break;
			case 8:		// Back
				excode = 1;
				break;
		}
		fifo->discard();
	} while (excode == 0);


	delete fifo;
	return 0;
	return 0;
}


int tbCoords(void)
{
	return 0;
}

int tbUnRedo(void)
{
	return 0;
}

// performers 
int tbPerfs_attr(void)
{
	return 0;
}
int tbPerfs_add(void)
{
	return 0;
}
int tbPerfs_del(void)
{
	return 0;
}


int tbSets_attr(void)
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

