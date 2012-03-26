#include <stdio.h>

int width;

void myfunc(void)
{
	width=2;
}
int main(void)
{
	myfunc();
	printf("%i", width);
	return 0;

}
