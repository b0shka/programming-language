#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int* remove_element(int* array, int sizeOfArray, int indexToRemove) {
	int* temp = malloc((sizeOfArray - 1) * sizeof(int));

	if (indexToRemove != 0)
		memcpy(temp, array, indexToRemove * sizeof(int));

	if (indexToRemove != (sizeOfArray - 1))
		memcpy(temp+indexToRemove, array+indexToRemove+1, (sizeOfArray - indexToRemove - 1) * sizeof(int));

	free(array);
	return temp;
}


int main() {
	int howMany = 20;
	int* test = malloc(howMany * sizeof(int*));

	for (int i = 0; i < howMany; ++i)
		test[i] = i;

	printf("%d\n", test[16]);
	test = remove_element(test, howMany, 16);
	printf("%d\n", test[16]);
	
	return 0;
}