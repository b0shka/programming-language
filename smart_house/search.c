#include <stdio.h>
#include <string.h>

int main (void)
{
   // Исходный массив
   unsigned char* src[15] = {"one", "two"};
   char *sym;

   sym = memchr(src, "one", 2);

   if (sym != NULL)
      printf("find!\n");
   return 0;
}
