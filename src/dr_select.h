typedef struct select_proto select_t;

// convenience functions
select_t *select_create(void);
int select_init(select_t *select);
void select_all_dots(void);

// basic external functions
// These don't interact with other structures
int select_get_dot_advance(select_t *select);
int select_get_form_advance(select_t *select);
int select_head(select_t *select);
int select_head_dot(select_t *select);
int select_head_form(select_t *select);
bool select_at_dot_end(select_t *select);
bool select_at_form_end(select_t *select);
bool select_at_end(select_t *select);
bool select_dot_empty(select_t *select);
bool select_form_empty(select_t *select);
bool select_empty(select_t *select);
void select_clear_dots(select_t *select);
void select_clear_forms(select_t *select);
void select_clear(select_t *select);
int select_get_dot(select_t *select);
int select_get_form(select_t *select);
int select_check_dot(select_t *select, int x);
int select_check_form(select_t *select, int x);
int select_add_dot(select_t *select, int x);
int select_add_form(select_t *select, int x);
int select_toggle_dot(select_t *select, int x);
int select_toggle_form(select_t *select, int x);
int select_remove_dot(select_t *select, int x);
int select_remove_form(select_t *select, int x);
int select_add_multiple(select_t *select, select_t *modifier);
int select_add_multiple_dots(select_t *select, select_t *modifier);
int select_add_multiple_forms(select_t *select, select_t *modifier);
int select_toggle_multiple(select_t *select, select_t *modifier);
int select_toggle_multiple_dots(select_t *select, select_t *modifier);
int select_toggle_multiple_forms(select_t *select, select_t *modifier);
int select_remove_multiple(select_t *select, select_t *modifier);
int select_remove_multiple_dots(select_t *select, select_t *modifier);
int select_remove_multiple_forms(select_t *select, select_t *modifier);
select_t *select_create_with_size(size_t dot_size, size_t form_size);
int select_init_with_size(select_t *select, int dot_size, int form_size);
select_t *select_destroy(select_t *select);
int select_update_center(select_t *select);

