#include "../dr_forms.h"
#include "../dr_form_parent.h"


form_parent_t *form_parent_construct(void)
{
	int i;
	form_parent_t *last;

	last = (form_parent_t*)malloc(sizeof(form_parent_t));
	last->forms = (form_child_t**)malloc(pshow->sets->size_alloc*sizeof(form_child_t*));
	for (i=0; i<pshow->sets->size_alloc; i++)
		last->forms[i] = NULL;
	return last;
}



form_parent_t *form_parent_construct_with_form(form_child_t *form, int index)
{
	form_parent_t *last;
	last = form_parent_construct();
	
	last->forms[0] = form;

	return last;
}


form_parent_t *form_parent_realloc(form_parent_t *form, int size, int size_realloc)
{
	int i;
	int newsize = (size < size_realloc ? size : size_realloc);
	form_parent_t *newform_parent;

	newform_parent = (form_parent_t*)malloc(size_realloc*sizeof(form_parent_t));
	for (i=0; i<newsize; i++)
		newform_parent->forms[i] = form->forms[i];
	free(form);
	return newform_parent;
}





form_parent_t *form_parent_destruct(form_parent_t *last)
{
	free(last->forms);
	//LIST_REMOVE(last, formlist_entries);
	free(last);
	return NULL;
}


bool form_parent_contiguous(form_parent_t *fparent, int set_num)
{
	int size;
	size = pshow->sets->size;
	if (set_num < size-1 && fparent->forms[set_num] && fparent->forms[set_num+1])
		return true;
	return false;
}

int form_parent_get_index(form_parent_t *form_parent)
{
	return form_parent->index;
}


form_container_t *form_parent_add_form(form_container_t *head, form_child_t *form, int index)
{
	form_parent_t *last;

	if (!head)
	{
		head = form_container_construct();
		//head = (form_list_t*)malloc(sizeof(form_list_t));
		//LIST_INIT(head);
	}
	
	last = form_parent_construct_with_form(form, index);
	//form->parent = last;
	form_child_set_parent(form, last);
	form_container_insert_head(head, last);

	//LIST_INSERT_HEAD(head, last, formlist_entries);

	return head;
}



int form_parent_add_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;
	int excode;
	if (!last)
		return -1;
	forms = last->forms;
	if (index > 0)
		forms[index] = form_copy(forms[index-1]);
	else
		forms[index] = form_child_construct(last);
	return 0;
	return excode;
}



int form_parent_copy_to(form_parent_t *last, int index, int transpose)
{
	form_child_t **forms;
	if (!last)
		return -1;
	forms = last->forms;
	forms[transpose] = form_copy(forms[index]);
	return 0;
}



int form_parent_remove_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;

	if (!last)
		return -1;
	forms = last->forms;
	if (forms[index])
	{
		form_child_destruct(forms[index]);
		forms[index] = NULL;
	}
	return 0;
}





form_child_t *form_parent_find_form_at_index(form_parent_t *last, int index)
{
	return last->forms[index];
}



bool form_parent_mapped_at_set(form_parent_t *last, int setnum)
{
	return (last->forms[setnum] != NULL);
}

form_container_t *form_container_construct(void)
{
	form_container_t *fcont;

	fcont = (form_container_t*)malloc(sizeof(form_container_t));
	fcont = form_container_realloc(fcont, 5);
	return fcont;
}


form_container_t *form_container_destruct(form_container_t *fcont)
{
	free(fcont->forms);
	free(fcont);
	return NULL;
}


form_container_t *form_container_realloc(form_container_t *fcont, size_t size_alloc)
{
	int i;
	form_parent_t **forms;
	size_t size;

	forms = (form_parent_t**)malloc(size_alloc*sizeof(form_parent_t*));
	size = fcont->size;
	for (i = 0; i < size; i++)
	{
		forms[i] = fcont->forms[i];
		if (forms[i])
			forms[i]->index = i;
	}
	free(fcont->forms);
	fcont->forms = forms;
	fcont->size_alloc = size_alloc;
	return fcont;
}

int form_container_get_size(form_container_t *fcont)
{
	return fcont->size;
}

void form_container_resize_parents(form_container_t *fcont, size_t oldsize, size_t newsize)
{
	int i;
	int parentnum = fcont->size;
	form_parent_t **parent = fcont->forms, **newparent;
	newparent = (form_parent_t**)malloc(newsize*sizeof(form_parent_t*));
	for (i=0; i < parentnum; i++)
		newparent[i] = form_parent_realloc(parent[i], oldsize, newsize);
	return;
}


void form_container_move_children_including(form_container_t *fcont, int setnum)
{
	int i, j;
	form_parent_t *parent;
	for(i=0; i<fcont->size; i++)
	{
		parent = fcont->forms[i];
		for (j=pshow->sets->size; j>=setnum; j--)
			parent->forms[i+1] = parent->forms[i];
	}
	return;
}




form_container_t *form_container_insert_head(form_container_t *fcont, form_parent_t *last)
{
	int i;
	int size;
	form_parent_t **forms;

	forms = fcont->forms;
	size = fcont->size;

	if (size+1 >= fcont->size_alloc)
		fcont = form_container_realloc(fcont, fcont->size_alloc+5);
	for (i=size-1; i>=0; i--)
	{
		forms[i+1] = forms[i];
		if (forms[i+1])
			forms[i+1]->index = i+1;
	}
	last->index = 0;
	forms[0] = last;
	fcont->size++;
	return fcont;
}

form_parent_t *form_container_get_form_parent(form_container_t *fcont, int index)
{
	return fcont->forms[index];
}

form_child_t *form_container_get_form_child_at_set(form_container_t *fcont, int index, int setnum)
{
	return form_container_get_form_parent(fcont, index)->forms[setnum];
}

form_child_t *form_container_get_form_child(form_container_t *fcont, int index)
{
	return form_container_get_form_parent(fcont, index)->forms[pstate.setnum];
}

