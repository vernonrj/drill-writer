typedef struct stepfield_structure stepfield_t;

bool stepfield_is_empty(stepfield_t *stepfield);
bool stepfield_at_end(stepfield_t *stepfield);
int stepfield_check(stepfield_t *stepfield, int x);
void stepfield_head(stepfield_t *stepfield);
int stepfield_retrieve(stepfield_t *stepfield);
int stepfield_retrieve_and_step(stepfield_t *stepfield);
int stepfield_add(stepfield_t *stepfield, int x);
int stepfield_toggle(stepfield_t *stepfield, int x);
int stepfield_remove(stepfield_t *stepfield, int x);
int stepfield_merge_left(stepfield_t *stepfield, stepfield_t *additions);
int stepfield_toggle_left(stepfield_t *stepfield, stepfield_t *modifier);
int stepfield_mask_left(stepfield_t *stepfield, stepfield_t *modifier);
void stepfield_clear(stepfield_t *stepfield);
stepfield_t *stepfield_create(int size);

