#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// internal functions
unsigned char bitfield_or(unsigned char index, unsigned char modifier);	// unconditionally add everything in modifier to index
unsigned char bitfield_xor(unsigned char index, unsigned char modifier);	// add modifier to index, drop anything that exists in both
unsigned char bitfield_mask(unsigned char index, unsigned char modifier);	// remove anything in modifier from index
int bitfield_map(unsigned char *bitfield, unsigned char *modifier, size_t size, unsigned char (*fptr)(unsigned char, unsigned char)); // execute any of the logic functions on the entire field
int bitfield_check(unsigned char *bitfield, size_t size_alloc, int x);	// check field for existance of x
int bitfield_encode(unsigned char *index, size_t size_alloc, int x, unsigned char (*fptr)(unsigned char, unsigned char));
int bitfield_decode(unsigned char *index, size_t size_alloc, int min_bit);

typedef struct stepfield_structure stepfield_t;
struct stepfield_structure
{
	// Step-through bitfield
	// bitfield that lets you walk through 
	// set bits like a linked list
	int index;
	unsigned char *field;	
	size_t fieldsize;
};


bool stepfield_is_empty(stepfield_t *stepfield)
{
	int i;
	int fieldsize = stepfield->fieldsize;
	for (i=0; i<fieldsize; i++)
	{
		if (stepfield->field[i] != 0)
			return false;
	}
	return true;
}


bool stepfield_at_end(stepfield_t *stepfield)
{
	return (stepfield->index == -1);
}


int stepfield_check(stepfield_t *stepfield, int x)
{
	return bitfield_check(stepfield->field, stepfield->fieldsize, x);
}

void stepfield_head(stepfield_t *stepfield)
{
	stepfield->index = 0;
}

int stepfield_retrieve(stepfield_t *stepfield)
{
	return bitfield_decode(stepfield->field, stepfield->fieldsize, 
			stepfield->index);
}

int stepfield_retrieve_and_step(stepfield_t *stepfield)
{
	int index = stepfield->index;
	if (stepfield->index == -1)
		return -1;
	index = stepfield_retrieve(stepfield);
	stepfield->index = (index != -1 ? index+1 : index);
	return index;
	/*
	if (index != -1)
		stepfield->index = index + 1;
	else
		stepfield->index = index;
	return index;
	*/
}

int stepfield_add(stepfield_t *stepfield, int x)
{
	return bitfield_encode(stepfield->field, 
			stepfield->fieldsize, x, &bitfield_or);
}


int stepfield_toggle(stepfield_t *stepfield, int x)
{
	return bitfield_encode(stepfield->field, 
			stepfield->fieldsize, x, &bitfield_xor);
}

int stepfield_remove(stepfield_t *stepfield, int x)
{
	return bitfield_encode(stepfield->field, 
			stepfield->fieldsize, x, &bitfield_mask);
}

int stepfield_merge_left(stepfield_t *stepfield, stepfield_t *additions)
{
	int fieldsize = (stepfield->fieldsize > additions->fieldsize ? 
			additions->fieldsize 
			: stepfield->fieldsize);
	bitfield_map(stepfield->field, 
			additions->field, fieldsize, &bitfield_or);
	return 0;
}

int stepfield_toggle_left(stepfield_t *stepfield, stepfield_t *modifier)
{
	int fieldsize = (stepfield->fieldsize > modifier->fieldsize ? 
			modifier->fieldsize 
			: stepfield->fieldsize);
	return bitfield_map(stepfield->field, modifier->field, 
			fieldsize, &bitfield_xor);
}


int stepfield_mask_left(stepfield_t *stepfield, stepfield_t *modifier)
{
	int fieldsize = (stepfield->fieldsize > modifier->fieldsize ? 
			modifier->fieldsize 
			: stepfield->fieldsize);
	return bitfield_map(stepfield->field, modifier->field, 
			fieldsize, &bitfield_mask);
}

void stepfield_clear(stepfield_t *stepfield)
{
	int i;
	unsigned char *field;
	size_t fieldsize;

	field = stepfield->field;
	fieldsize = stepfield->fieldsize;
	for (i=0; i<fieldsize; i++)
		field[i] = 0x0;
	stepfield->index = 0;
	return;
}

stepfield_t *stepfield_create(int size)
{
	stepfield_t *stepfield;
	size_t alloc_size;

	stepfield = (stepfield_t*)malloc(sizeof(stepfield_t));
	if (stepfield == NULL)
	{
		printf("ERROR: Allocation\n");
		exit(EXIT_FAILURE);
	}

	alloc_size = size/8 + 1;

	stepfield->field = (unsigned char*)malloc(
			alloc_size*sizeof(unsigned char));
	stepfield->fieldsize = alloc_size;
	stepfield_clear(stepfield);

	return stepfield;
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

