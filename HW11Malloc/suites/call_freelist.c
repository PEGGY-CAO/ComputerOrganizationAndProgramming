#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "suites.h"

#define ELEMS 10
#define SUBPTR(a, b) ((((uint8_t*)(a)) - ((uint8_t*)(b))) + 1) 

extern uint8_t *my_sbrk_fake_heap;

uintptr_t pp_pointer(metadata_t* ptr) {
	if (ptr == NULL) {
		return (uintptr_t)(NULL);
	}
	return (uintptr_t)((uint8_t*)ptr - (uint8_t*)my_sbrk_fake_heap + 1);
}

void call_freelist(const char* filename) {
	FILE *outFP;
	size_t size_sbrk = SBRK_SIZE * (my_sbrk_imaginary_prev_calls + 1);
	outFP = fopen(filename, "w");
	fprintf(outFP, "************ACT ADDRESS LIST************\n");
	fprintf(outFP, "%-40s\n", "");
	fflush(outFP);
	metadata_t* curr = address_list;
	metadata_t** addr_seen = calloc(ELEMS, sizeof(metadata_t));
	while(curr) {
		for (int i = 0; i < ELEMS; i++) {
			if (addr_seen[i] == 0) {
				addr_seen[i] = curr;
				break;
			} else if (addr_seen[i] == curr) {
				fprintf(outFP, "ERROR: INFINITE LOOP DETECTED\n");
				break;
				//fclose(outFP);
			}
		}
		
		if ((uintptr_t)curr < (uintptr_t)my_sbrk_fake_heap || (uintptr_t)curr >= (uintptr_t)((uint8_t*)my_sbrk_fake_heap + size_sbrk - sizeof (metadata_t))) {
			fprintf(outFP, "ERROR: POINTER OUTSIDE OF THE HEAP\n");
			//fclose(outFP);
			break;
		}
	
		fprintf(outFP, "Address: %-32p\n", (void*)(SUBPTR(curr, my_sbrk_fake_heap)));
		fprintf(outFP, "Size: %-35lu\n", curr -> size);
		if (curr -> prev_addr == NULL) {
			fprintf(outFP, "PrevAddr: %-29p\n", (void*)curr -> prev_addr);
		} else {
			fprintf(outFP, "PrevAddr: %-29p\n", (void*)(SUBPTR(curr -> prev_addr, my_sbrk_fake_heap)));
		}
		if (curr -> next_addr == NULL) {
			fprintf(outFP, "NextAddr: %-29p\n", (void*)curr -> next_addr);
		} else {
			fprintf(outFP, "NextAddr: %-29p\n", (void*)(SUBPTR(curr -> next_addr, my_sbrk_fake_heap)));
		}
		fprintf(outFP, "%-40s\n", "");
		fflush(outFP);
		curr = curr -> next_addr;
	}

	fprintf(outFP, "************* ACT SIZE LIST*************\n");
	fprintf(outFP, "%-40s\n", "");
	fflush(outFP);
	curr = size_list;
	metadata_t** size_seen = calloc(ELEMS, sizeof(metadata_t));
	while(curr) {
		for (int i = 0; i < ELEMS; i++) {
			if (size_seen[i] == 0) {
				size_seen[i] = curr;
				break;
			} else if (size_seen[i] == curr) {
				fprintf(outFP, "ERROR: INFINITE LOOP DETECTED\n");
				//fclose(outFP);
				break;
			}
		}
		
		if ((uintptr_t)curr < (uintptr_t)my_sbrk_fake_heap || (uintptr_t)curr >= (uintptr_t)((uint8_t*)my_sbrk_fake_heap + size_sbrk - sizeof (metadata_t))) {
			fprintf(outFP, "ERROR: POINTER OUTSIDE OF THE HEAP\n");
			//fclose(outFP);
			break;
		}
		
		fprintf(outFP, "Address: %-32p\n", (void*)(SUBPTR(curr, my_sbrk_fake_heap)));
		fprintf(outFP, "Size: %-35lu\n", curr -> size);
		if (curr -> prev_size == NULL) {
			fprintf(outFP, "PrevSize: %-29p\n", (void*)curr -> prev_size);
		} else {
			fprintf(outFP, "PrevSize: %-29p\n", (void*)(SUBPTR(curr -> prev_size, my_sbrk_fake_heap)));
		}
		if (curr -> next_size == NULL) {
			fprintf(outFP, "NextSize: %-29p\n", (void*)curr -> next_size);
		} else {
			fprintf(outFP, "NextSize: %-29p\n", (void*)(SUBPTR(curr -> next_size, my_sbrk_fake_heap)));
		}
		fprintf(outFP, "%-40s\n", "");
		fflush(outFP);
		curr = curr -> next_size;
	}
	fclose(outFP);
}
