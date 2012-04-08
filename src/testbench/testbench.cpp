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
	return 0;
}

int tbSets(void)
{
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
