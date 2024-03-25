#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	//printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	//printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

// implements heap sort
// extraMemoryAllocated counts bytes of memory allocated
void heapSort(int arr[], int n)
{
	int i, j, max, temp;
	// Build heap (rearrange array)
	for (i = n / 2 - 1; i >= 0; i--) {
		// Heapify subtree rooted at index i
		for (j = i; j * 2 + 1 < n; j = max) {
			max = j * 2 + 1;
			if (max + 1 < n && arr[max] < arr[max + 1]) // If right child exists and is greater
				max++;
			if (arr[j] < arr[max]) { // If largest child is greater than parent
				temp = arr[j]; // Swap parent with largest child
				arr[j] = arr[max];
				arr[max] = temp;
			}
			else
				break; // If parent is larger than any children, heapify is done
		}
	}

	// Extract elements from heap one by one
	for (i = n - 1; i > 0; i--) {
		// Move current root to end
		temp = arr[0];
		arr[0] = arr[i];
		arr[i] = temp;
		// Call max heapify on the reduced heap
		for (j = 0; j * 2 + 1 < i; j = max) {
			max = j * 2 + 1;
			if (max + 1 < i && arr[max] < arr[max + 1]) // If right child exists and is greater
				max++;
			if (arr[j] < arr[max]) { // If largest child is greater than parent
				temp = arr[j]; // Swap parent with largest child
				arr[j] = arr[max];
				arr[max] = temp;
			}
			else
				break; // If parent is larger than any children, heapify is done
		}
	}
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
	if (l < r)
	{
		// Get the mid point
		int m = (l + r) / 2;
		// Sort first and second halves
		mergeSort(pData, l, m);
		mergeSort(pData, m + 1, r);

		int i, j, k;
		int n1 = m - l + 1;
		int n2 = r - m;
		/* create temp arrays */
		int* L = Alloc(n1 * sizeof(int));
		int* R = Alloc(n2 * sizeof(int));
		/* Copy data to temp arrays L[] and R[] */
		for (i = 0; i < n1; i++)
			L[i] = pData[l + i];
		for (j = 0; j < n2; j++)
			R[j] = pData[m + 1 + j];
		/* Merge the temp arrays back into arr[l..r]*/
		i = 0; // Initial index of first subarray
		j = 0; // Initial index of second subarray
		k = l; // Initial index of merged subarray
		while (i < n1 && j < n2)
		{
			if (L[i] <= R[j])
			{
				pData[k] = L[i];
				i++;
			}
			else
			{
				pData[k] = R[j];
				j++;
			}
			k++;
		}
		//Copy the remaining elements of L[], if there are any.
		while (i < n1)
		{
			pData[k] = L[i];
			i++;
			k++;
		}
		// Copy the remaining elements of R[], if there are any.
		while (j < n2)
		{
			pData[k] = R[j];
			j++;
			k++;
		}
		DeAlloc(L);
		DeAlloc(R);
	}
}

// implement insertion sort
// extraMemoryAllocated counts bytes of memory allocated
void insertionSort(int* pData, int n)
{
	int minIndex = 0;
	for (int i = 1; i < n; i++) {
		int key = pData[i];
		int j = i - 1;
		while (j >= 0 && pData[j] > key) {
			pData[j + 1] = pData[j];
			j = j - 1;
		}
		pData[j + 1] = key;
	}
}

// implement bubble sort
// extraMemoryAllocated counts bytes of extra memory allocated
void bubbleSort(int* pData, int n)
{
	int temp = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - 1; j++) {
			if (pData[j] > pData[j + 1]) {
				temp = pData[j];
				pData[j] = pData[j + 1];
				pData[j + 1] = temp;
			}
		}
	}
}

// implement selection sort
// extraMemoryAllocated counts bytes of extra memory allocated
void selectionSort(int* pData, int n)
{
	int minIndex = 0;
	int minNumber = 0;
	int temp = 0;
	for (int i = 0; i < n - 1; i++) {
		minIndex = i;
		for (int j = i + 1; j < n; j++) {
			if (pData[j] < pData[minIndex])
				minIndex = j;
		}
		if (minIndex != i) {
			temp = pData[i];
			pData[i] = pData[minIndex];
			pData[minIndex] = temp;
		}
	}
}

// parses input file to an integer array
int parseData(char* inputFileName, int** ppData)
{
	FILE* fptr = fopen(inputFileName, "r");
	int dataSz = 0;
	int i, n, * data;
	*ppData = NULL;

	if (fptr)
	{
		fscanf(fptr, "%d\n", &dataSz);
		*ppData = (int*)Alloc(sizeof(int) * dataSz);
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i = 0; i < dataSz; ++i)
		{
			fscanf(fptr, "%d ", &n);
			data = *ppData + i;
			*data = n;
		}

		fclose(fptr);
	}

	return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
	int i, sz = (dataSz > 100 ? dataSz - 100 : 0);
	int firstHundred = (dataSz < 100 ? dataSz : 100);
	printf("\tData:\n\t");
	for (i = 0; i < firstHundred; ++i)
	{
		printf("%d ", pData[i]);
	}
	printf("\n\t");

	for (i = sz; i < dataSz; ++i)
	{
		printf("%d ", pData[i]);
	}
	printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
	double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt" };

	for (i = 0; i < 3; ++i)
	{
		int* pDataSrc, * pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);

		if (dataSz <= 0)
			continue;

		pDataCopy = (int*)Alloc(sizeof(int) * dataSz);

		printf("---------------------------\n");
		printf("Dataset Size : %d\n", dataSz);
		printf("---------------------------\n");
		
		printf("Selection Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		selectionSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Insertion Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		insertionSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Bubble Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		bubbleSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);
		
		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

	
		printf("Heap Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		heapSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		DeAlloc(pDataCopy);
		DeAlloc(pDataSrc);
	}
}