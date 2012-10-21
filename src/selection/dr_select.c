// Use a bitfield for selection purposes
// Using a bitfield has many advantages over a standard linked list
// 	+ Fewer allocs
// 		+ allocs only needed at beginning, not needed when new index added
// 	+ much quicker comparison between selections (ie what is in selection group A that's in group B also?)
// 	+ All actions are algorithmically faster with the bitfield,
// 		and should be real-world faster too
// 	Action		Bitfield	List
// 	- Adding	O(1)		O(n)
// 	- Removing	O(1)		O(n)
// 	- Checking	O(1)		O(n)
// 	- Increm.	O(logn)		O(n)
// 	- Compare	O(logn)		O(n)
//
// This also encapsulates select_proto, so future improvements should be easier.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../dr_select.h"
#include "../structures.h"
#include "../dr_forms.h"
#include "../drillwriter.h"	// coords structure



// internal functions
unsigned char bitfield_or(unsigned char index, unsigned char modifier);	// unconditionally add everything in modifier to index
unsigned char bitfield_xor(unsigned char index, unsigned char modifier);	// add modifier to index, drop anything that exists in both
unsigned char bitfield_mask(unsigned char index, unsigned char modifier);	// remove anything in modifier from index
int bitfield_map(unsigned char *bitfield, unsigned char *modifier, size_t size, unsigned char (*fptr)(unsigned char, unsigned char)); // execute any of the logic functions on the entire field
int bitfield_check(unsigned char *bitfield, size_t size_alloc, int x);	// check field for existance of x
int bitfield_encode(unsigned char *index, size_t size_alloc, int x, unsigned char (*fptr)(unsigned char, unsigned char));
int bitfield_decode(unsigned char *index, size_t size_alloc, int min_bit);

struct select_proto
{
	int dot_index;
	int form_index;
	unsigned char *dotfield;	// field for dots
	unsigned char *formfield;	// field for forms
	size_t dot_alloc;		// allocation size of dotfield
	size_t form_alloc;		// allocation size of formfield
};



int select_update_center(select_t *select)
{
	int index;
	double x, y, minx = -1, miny = -1, maxx = 0, maxy = 0;
	int setnum = pstate.setnum;
	double formxmin, formxmax, formymin, formymax;
	form_child_t *form;

	select_head(select);
	while ((index = select_get_dot_advance(select)) != -1)
	{
		coords_retrieve_midset(setnum, index, &x, &y);
		if (minx == -1 || x < minx)
			minx = x;
		if (x > maxx)
			maxx = x;
		if (miny == -1 || y < miny)
			miny = y;
		if (y > maxy)
			maxy = y;
	}
	while ((index = select_get_form_advance(select)) != -1)
	{
		form = form_container_get_form_child(pshow->topforms, index);
		form_get_dimensions(form, &formxmin, &formymin, &formxmax, &formymax);
		minx = (formxmin < minx || minx == -1 ? formxmin : minx);
		maxx = (formxmax > maxx ? formxmax : maxx);
		miny = (formymin < miny || miny == -1 ? formymin : miny);
		maxy = (formymax > maxy ? formymax : maxy);
	}
	pstate.center->x = ((maxx - minx) / 2) + minx;
	pstate.center->y = ((maxy - miny) / 2) + miny;
	select_head(select);
	return 0;
}

	


select_t *select_create(void)
{
	// create and initialize memory with default size
	int dot_size;
	int form_size;

	dot_size = pshow->perfnum;
	form_size = pshow->topforms ? pshow->topforms->size : 0;

	return select_create_with_size(dot_size, form_size);
}


int select_init(select_t *select)
{
	if (!select)
		return -1;
	select_init_with_size(select, pshow->perfnum, pshow->topforms->size);
	return 0;
}

void select_all_dots(void)
{
	int i;
	perf_t **perfs = pshow->perfs;
	int perfnum = pshow->perfnum;
	select_t *select = pstate.select;

	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid)
			select_add_dot(select, i);
	}
	return;
}




int select_get_dot_advance(select_t *select)
{
	int dot_index = select->dot_index;
	if (select->dot_index == -1)
		return -1;
	dot_index = select_get_dot(select);
	if (dot_index != -1)
		select->dot_index = dot_index + 1;
	else
		select->dot_index = dot_index;
	return dot_index;
}


int select_get_form_advance(select_t *select)
{
	int form_index = select->form_index;
	if (form_index == -1)
		return -1;
	form_index = select_get_form(select);
	if (form_index != -1)
		select->form_index = form_index + 1;
	else
		select->form_index = form_index;
	return form_index;
}

int select_head(select_t *select)
{
	select_head_dot(select);
	select_head_form(select);
	return 0;
}

int select_head_dot(select_t *select)
{
	select->dot_index = 0;
	return 0;
}


int select_head_form(select_t *select)
{
	select->form_index = 0;
	return 0;
}


bool select_at_dot_end(select_t *select)
{
	return (select->dot_index == -1);
}


bool select_at_form_end(select_t *select)
{
	return (select->form_index == -1);
}


bool select_at_end(select_t *select)
{
	return (select_at_dot_end(select) && select_at_form_end(select));
}

bool select_dot_empty(select_t *select)
{
	int i;
	int dot_alloc = select->dot_alloc;
	for (i=0; i<dot_alloc; i++)
	{
		if (select->dotfield[i] != 0)
			return false;
	}
	return true;
}


bool select_form_empty(select_t *select)
{
	int i;
	int form_alloc = select->form_alloc;
	for (i=0; i<form_alloc; i++)
	{
		if (select->formfield[i] != 0)
			return false;
	}
	return true;
}

bool select_empty(select_t *select)
{
	return (select_dot_empty(select) && select_form_empty(select));
}


int select_get_dot(select_t *select)
{
	// get the next dot >= min_bit
	return bitfield_decode(select->dotfield, select->dot_alloc, select->dot_index);
}

int select_get_form(select_t *select)
{
	// get the next form >= min_bit
	return bitfield_decode(select->formfield, select->form_alloc, select->form_index);
}

int select_check_dot(select_t *select, int x)
{
	// check if dot x exists in select
	return bitfield_check(select->dotfield, select->dot_alloc, x);
}

int select_check_form(select_t *select, int x)
{
	// check if form x exists in select
	return bitfield_check(select->formfield, select->form_alloc, x);
}

int select_add_dot(select_t *select, int x)
{
	// add dot x to select
	return bitfield_encode(select->dotfield, select->dot_alloc, x, &bitfield_or);
}

int select_add_form(select_t *select, int x)
{
	// add form x to select
	return bitfield_encode(select->formfield, select->form_alloc, x, &bitfield_or);
}

int select_toggle_dot(select_t *select, int x)
{
	// toggle dot x in select
	return bitfield_encode(select->dotfield, select->dot_alloc, x, &bitfield_xor);
}

int select_toggle_form(select_t *select, int x)
{
	// toggle form x in select
	return bitfield_encode(select->formfield, select->form_alloc, x, &bitfield_xor);
}

int select_remove_dot(select_t *select, int x)
{
	// remove dot x from select
	return bitfield_encode(select->dotfield, select->dot_alloc, x, &bitfield_mask);
}

int select_remove_form(select_t *select, int x)
{
	// remove form x from select
	return bitfield_encode(select->formfield, select->form_alloc, x, &bitfield_mask);
}

int select_add_multiple(select_t *select, select_t *modifier)
{
	// add everything from modifier to select
	int dot_alloc = (select->dot_alloc > modifier->dot_alloc ? modifier->dot_alloc : select->dot_alloc);
	int form_alloc = (select->form_alloc > modifier->form_alloc ? modifier->form_alloc : select->form_alloc);
	bitfield_map(select->dotfield, modifier->dotfield, dot_alloc, &bitfield_or);
	bitfield_map(select->formfield, modifier->formfield, form_alloc, &bitfield_or);
	return 0;
}


int select_add_multiple_dots(select_t *select, select_t *modifier)
{
	// add all dots from modifier to select
	return bitfield_map(select->dotfield, modifier->dotfield, select->dot_alloc, &bitfield_or);
}


int select_add_multiple_forms(select_t *select, select_t *modifier)
{
	// add all forms from modifier to select
	return bitfield_map(select->formfield, modifier->formfield, select->form_alloc, &bitfield_or);
}


int select_toggle_multiple(select_t *select, select_t *modifier)
{
	// toggle everything from modifier to select
	int dot_alloc = (select->dot_alloc > modifier->dot_alloc ? modifier->dot_alloc : select->dot_alloc);
	int form_alloc = (select->form_alloc > modifier->form_alloc ? modifier->form_alloc : select->form_alloc);
	bitfield_map(select->dotfield, modifier->dotfield, dot_alloc, &bitfield_or);
	bitfield_map(select->formfield, modifier->formfield, form_alloc, &bitfield_or);
	return 0;
}

int select_toggle_multiple_dots(select_t *select, select_t *modifier)
{
	// toggle all dots from modifier in select
	return bitfield_map(select->dotfield, modifier->dotfield, select->dot_alloc, &bitfield_xor);
}


int select_toggle_multiple_forms(select_t *select, select_t *modifier)
{
	// toggle all forms from modifier in select
	return bitfield_map(select->formfield, modifier->formfield, select->form_alloc, &bitfield_xor);
}


int select_remove_multiple(select_t *select, select_t *modifier)
{
	int dot_alloc = (select->dot_alloc > modifier->dot_alloc ? modifier->dot_alloc : select->dot_alloc);
	int form_alloc = (select->form_alloc > modifier->form_alloc ? modifier->form_alloc : select->form_alloc);
	bitfield_map(select->dotfield, modifier->dotfield, dot_alloc, &bitfield_mask);
	bitfield_map(select->formfield, modifier->formfield, form_alloc, &bitfield_mask);
	return 0;
}

int select_remove_multiple_dots(select_t *select, select_t *modifier)
{
	// remove all dots in modifier from select
	return bitfield_map(select->dotfield, modifier->dotfield, select->dot_alloc, &bitfield_mask);
}


int select_remove_multiple_forms(select_t *select, select_t *modifier)
{
	// remove all forms in modifier from select
	return bitfield_map(select->formfield, modifier->formfield, select->form_alloc, &bitfield_mask);
}

void select_clear_dots(select_t *select)
{
	// remove all dots from select
	int i;
	unsigned char *dotfield;
	size_t dot_alloc;

	if (!select)
		return;
	dotfield = select->dotfield;
	dot_alloc = select->dot_alloc;
	for (i=0; i<dot_alloc; i++)
		dotfield[i] = 0x0;
	select->dot_index = 0;
	return;
}

void select_clear_forms(select_t *select)
{
	// remove all dots from select
	int i;
	unsigned char *formfield;
	size_t form_alloc;

	if (!select)
		return;
	formfield = select->formfield;
	form_alloc = select->form_alloc;
	for (i=0; i<form_alloc; i++)
		formfield[i] = 0x0;
	select->form_index = 0;

	return;
}




void select_clear(select_t *select)
{
	// remove all dots and forms from select
	int i;
	unsigned char *dotfield;
	unsigned char *formfield;
	size_t dot_alloc;
	size_t form_alloc;

	if (!select)
		return;
	dotfield = select->dotfield;
	formfield = select->formfield;
	dot_alloc = select->dot_alloc;
	form_alloc = select->form_alloc;
	for (i=0; i<dot_alloc; i++)
		dotfield[i] = 0x0;
	for (i=0; i<form_alloc; i++)
		formfield[i] = 0x0;
	select->dot_index = 0;
	select->form_index = 0;

	return;
}


select_t *select_create_with_size(size_t dot_size, size_t form_size)
{
	// create and initialize memory
	// with a specified size
	select_t *select;

	select = (select_t*)malloc(sizeof(select_t));
	if (select == NULL)
	{
		printf("ERROR: Allocation\n");
		exit(EXIT_FAILURE);
	}

	select_init_with_size(select, dot_size, form_size);

	return select;
}




int select_init_with_size(select_t *select, int dot_alloc, int form_alloc)
{
	int dot_size = dot_alloc/8 + 1;
	int form_size = form_alloc/8 + 1;
	if (!select)
		return -1;

	select->dotfield = (unsigned char*)malloc(dot_size*sizeof(unsigned char));
	select->formfield = (unsigned char*)malloc(form_size*sizeof(unsigned char));
	select->dot_alloc = dot_size;
	select->form_alloc = form_size;
	select_clear(select);
	return 0;
}




select_t *select_destroy(select_t *select)
{
	if (!select)
		return NULL;

	free(select->formfield);
	free(select->dotfield);
	free(select);

	return NULL;
}



// internal functions

unsigned char bitfield_or(unsigned char bitfield, unsigned char modifier)
{	// return the inclusive or of two bytes
	return (bitfield | modifier);
}


unsigned char bitfield_xor(unsigned char bitfield, unsigned char modifier)
{	// return the exclusive or of two bytes
	return (bitfield ^ modifier);
}


unsigned char bitfield_mask(unsigned char bitfield, unsigned char modifier)
{	// remove any bit from field that exists in modifier
	return (bitfield & (~modifier));
}


int bitfield_map(unsigned char *bitfield, unsigned char *modifier, size_t size, unsigned char (*fptr)(unsigned char, unsigned char))
{
	// execute any field logic function on the entire field
	int i;
	for (i=0; i<size; i++)
		bitfield[i] = fptr (bitfield[i], modifier[i]);
	return 0;
}



int bitfield_check(unsigned char *bitfield, size_t size_alloc, int x)
{
	// check field for existence of x
	int index;
	unsigned char coded;

	index = x >> 3;
	if (index >= size_alloc)
		return -1;

	x = x & 0x7;
	coded = 1 << x;

	return ((bitfield[index] & coded) != 0x0);
}



int bitfield_encode(unsigned char *bitfield, size_t size_alloc, int x, unsigned char (*fptr)(unsigned char, unsigned char))
{
	// encode x into field using function provided by fptr
	int index;		// character array offset
	unsigned char coded;	// encoded number

	index = x >> 3;
	if (index >= size_alloc)
		return -1;

	x = x & 0x7;
	coded = 1 << x;

	bitfield[index] = fptr (bitfield[index], coded);

	return 0;
}



int bitfield_decode(unsigned char *bitfield, size_t size_alloc, int min_bit)
{
	// mask everything < min_bit
	// decode next smallest number
	// return -1 if no number
	
	int index;		// character array offset
	unsigned char coded;	// encoded number
	int x;			// masked number
	int dec_out = 0;	// fully decoded relative minimum

	index = min_bit >> 3;
	if (index >= size_alloc)
		return -1;

	coded = (1 << (min_bit & 0x7)) - 1;
	do
	{
		x = bitfield[index] & ~coded;
		if (!x)
		{
			index++;
			coded = 0;
			if (index >= size_alloc)
				return -1;
		}
	} while (!x);
	x = x & (~x+1);		// get encoded relative minimum

	// decode
	dec_out = 8*index;
	dec_out += (((x & 0xAA) != 0x0) << 0);
	dec_out += (((x & 0xCC) != 0x0) << 1);
	dec_out += (((x & 0xF0) != 0x0) << 2);

	return dec_out;
}
