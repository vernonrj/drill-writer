#include "drillwriter.h"
#include "dr_select.h"


group_t *group_construct(void)
{
	// make a group node
	group_t *group;
	group = (group_t*)malloc(sizeof(group_t));
	if (!group)
		return NULL;
	group->selects = select_create();
	group->next = NULL;
	group->name = (char*)malloc(10*sizeof(char));
	group->local = false;
	snprintf(group->name, 10, "New Group");

	return group;
}



group_t *group_add_selects(group_t *group, select_t *newsels)
{
	// add selects to group
	//select_t *scurr = newsels;
	//select_t *glast;
	//select_t *select_new = NULL;
	//int sindex, gindex;

	if (!group)
		group = group_construct();
	select_add_multiple_dots(group->selects, newsels);

	/*
	while (scurr && glast)
	{
		// merge selection and group selects in order
		//if (scurr->index < glast->index)
		sindex = select_get_dot(scurr);
		gindex = select_get_dot(glast);
		if (sindex < gindex)
		{
			// selection goes next
			select_new = select_add_index(select_new, sindex, false);
			//scurr = scurr->next;
			scurr = select_get_next(scurr);
		}
		else if (sindex > gindex)
		{
			// group goes next
			select_new = select_add_index(select_new, gindex, false);
			//glast = glast->next;
			glast = select_get_next(glast);
		}
		else
		{
			// duplicate
			//scurr = scurr->next;
			scurr = select_get_next(scurr);
		}
	}
	// clean up
	while (scurr)
	{
		//select_new = select_add_index(select_new, scurr->index, false);
		//scurr = scurr->next;
		select_new = select_add_index(select_new, select_get_dot(scurr), false);
		scurr = select_get_next(scurr);
	}
	while (glast)
	{
		//select_new = select_add_index(select_new, glast->index, false);
		//glast = glast->next;
		select_new = select_add_index(select_new, select_get_dot(glast), false);
		glast = select_get_next(glast);
	}
	// add new selection to group
	select_discard(group->selects);
	group->selects = select_new;
	*/
	return group;
}


group_t *group_remove_selects(group_t *group, select_t *select)
{
	if (!group || !select)
		return group;
	//group->selects = select_drop_multiple(group->selects, select);
	select_remove_multiple_dots(group->selects, select);
	return group;
}



group_t *group_remove_from(group_t *group, group_t *curr)
{
	// remove group from the current
	// return current

	if (group == curr)
	{
		curr = group_pop_from(group, curr);
	}
	else
		group_pop_from(group, curr);
	free(group);
	return curr;
}



group_t *group_pop_from(group_t *group, group_t *curr)
{
	// pop group out of current
	// so group is still allocated,
	// but is not in the curr list
	// return current

	group_t *last = curr;
	if (!group)
		return NULL;
	if (!curr)
		return NULL;
	if (last != group)
	{
		while (last && last->next != group)
			last = last->next;
	}
	else
	{
		last = last->next;
		//free(group);
		return last;
	}

	if (!last)
		return NULL;

	last->next = group->next;
	group->next = NULL;
	return curr;

}


void group_add_to_set(group_t *group)
{
	// add a group to the local set of groups

	set_t *sets = pshow->sets->currset;
	group_t *setgroup = sets->groups;
	group->next = NULL;
	if (sets->groups == NULL)
	{
		sets->groups = group;
		return;
	}
	while (setgroup->next)
		setgroup = setgroup->next;
	setgroup->next = group;
	return;
}


void group_add_global(group_t *group)
{
	// add a group to the global set
	group_t *last = pshow->topgroups;
	if (last == NULL)
	{
		pshow->topgroups = group;
		return;
	}
	while (last->next)
		last = last->next;
	last->next = group;
	return;
}

/*
bool group_is_selected(group_t *group, select_t *select)
{
	while (select)
	{
		if (select->group == group)
			return true;
		select = select->next;
	}
	return false;
}
*/
