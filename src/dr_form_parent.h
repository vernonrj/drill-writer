typedef struct _form_parent_t form_parent_t;
typedef struct _form_container_t form_container_t;

struct _form_parent_t
{
	// container for forms
	//LIST_ENTRY(_form_parent_t) formlist_entries;
	//int* set_index;
	form_child_t **forms;
	int index;
	//int size;
	//int size_alloc;
};


struct _form_container_t
{
	form_parent_t **forms;
	int size;
	int size_alloc;
};

form_parent_t *form_parent_construct(void);
form_parent_t *form_parent_construct_with_form(form_child_t *form, int index);
form_parent_t *form_parent_realloc(form_parent_t *form, int size, int size_realloc);
form_parent_t *form_parent_destruct(form_parent_t *last);


bool form_parent_contiguous(form_parent_t *fparent, int set_num);

int form_parent_get_index(form_parent_t *form_parent);
form_container_t *form_parent_add_form(form_container_t *head, form_child_t *form, int index);
int form_parent_add_set(form_container_t *head, form_parent_t *last, int index);
int form_parent_copy_to(form_parent_t *last, int index, int transpose);
int form_parent_remove_set(form_container_t *head, form_parent_t *last, int index);
form_child_t *form_parent_find_form_at_index(form_parent_t *last, int index);
bool form_parent_mapped_at_set(form_parent_t *last, int setnum);





/* form container */
form_container_t *form_container_construct(void);
form_container_t *form_container_destruct(form_container_t *fcont);
form_container_t *form_container_realloc(form_container_t *fcont, size_t size_alloc);

int form_container_get_size(form_container_t *fcont);
void form_container_resize_parents(form_container_t *fcont, size_t oldsize, size_t newsize);
void form_container_move_children_including(form_container_t *fcont, int setnum);
form_container_t *form_container_insert_head(form_container_t *fcont, form_parent_t *last);
form_parent_t *form_container_get_form_parent(form_container_t *fcont, int index);
form_child_t *form_container_get_form_child_at_set(form_container_t *fcont, int index, int setnum);
form_child_t *form_container_get_form_child(form_container_t *fcont, int index);

