extern "C" {
	#include "drill.h"
}
#include <iostream>
#include "util.hpp"
extern "C" int foo(void);
int mainmenu(void);
int tbPerfs(void);
int tbSets(void);
int tbUnRedo(void);

int tbPerfs_attr(void);
int tbPerfs_add(void);
int tbPerfs_del(void);

int tbSets_attr(void);
int tbCoords(void);
int tbSets_add(void);
int tbSets_del(void);

int tbCoords_view(void);
int tbSelect(void);
int tbCoords_change(void);

int tbSelect_view(void);
int tbSelect_add(void);
int tbSelect_rem(void);

