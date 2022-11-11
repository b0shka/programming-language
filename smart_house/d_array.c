#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
 char **strarray = NULL;
 int i = 0, strcount = 0;
 char line[1024];
 
 while((fgets(line, 1024, stdin)) != NULL) {
	strarray = (char **)realloc(strarray, (strcount + 1) * sizeof(char *));
	strarray[strcount++] = strdup(line);

	for(i = 0; i < strcount; i++)
		printf("strarray[%d] == %s", i, strarray[i]);
 }
 /* 
 // free the string array
 // Note: You must delete each individual string 
 //       before you delete the array of pointers
 */
 for(i = 0; i < strcount; i++)
  free(strarray[i]);

 free(strarray);
 return 0;
}
