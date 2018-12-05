/*
 * CS 2110 Fall 2018
 * Author: Yuqi Cao
 */

/* we need this for uintptr_t */
#include <stdint.h>
/* we need this for memcpy/memset */
#include <string.h>
/* we need this to print out stuff*/
#include <stdio.h>
/* we need this for the metadata_t struct and my_malloc_err enum definitions */
#include "my_malloc.h"
/* include this for any boolean methods */
#include <stdbool.h>


/* Our freelist structure - our freelist is represented as two doubly linked lists
 * the address_list orders the free blocks in ascending address
 * the size_list orders the free blocks by size
 */

metadata_t *address_list;
metadata_t *size_list;
/*Function Headers
 * Here is a place to put all of your function headers
 * Remember to declare them as static
 */

#define GETCAN(block) *((unsigned long*) ((uint8_t*)(block) + (block)->size - sizeof(unsigned long)))

#define SETCAN(block) GETCAN(block)=(block)->canary

static void removeFromAlist(metadata_t *block) {
	if (block->prev_addr) {
		block->prev_addr->next_addr = block->next_addr;
	} else {
		address_list = block->next_addr;
	}
	if (block->next_addr) {
		block->next_addr->prev_addr = block->prev_addr;
	}
	block->prev_addr = 0;
	block->next_addr = 0;
}

static void removeFromSizeList(metadata_t *block) {
	if (block->prev_size) {
		block->prev_size->next_size = block->next_size;
	} else {
		size_list = block->next_size;
	}
	if (block->next_size) {
		block->next_size->prev_size = block->prev_size;
	}
	block->prev_size = 0;
	block->next_size = 0;
}

static metadata_t *splitB(metadata_t *block, size_t size) {
	size_t realSize = size + TOTAL_METADATA_SIZE;
	block->size -= realSize;
	block->canary = (((uintptr_t)block)^CANARY_MAGIC_NUMBER) + 1;
	SETCAN(block);
	metadata_t *n = (metadata_t *)((uint8_t*)(block) + block->size);
	n->prev_addr = NULL;
	n->next_addr = NULL;
	n->prev_size = NULL;
	n->next_size = NULL;
	n->size = realSize;
	n->canary = (((uintptr_t)n)^CANARY_MAGIC_NUMBER) + 1;
	SETCAN(n);
	metadata_t *i = block->prev_size;
	metadata_t *j = block->next_size;
	if (i && i->size > block->size) {
		i->next_size = j;
		if (j) j->prev_size = i;
		for (;i && i->size > block->size; i = i->prev_size);
		block->prev_size = i;
		if (i) {
			block->next_size = i->next_size;
			block->next_size->prev_size = block;
			i->next_size = block;
		} else {
			block->next_size = size_list;
			size_list->prev_size = block;
			size_list = block;
		}
	}
	return n+1;

}


static bool doubleMergeAlist(metadata_t *left, metadata_t *mid, metadata_t *right) {
	if (left && mid && right && (uintptr_t)left + left->size == (uintptr_t)mid && (uintptr_t)mid + mid->size == (uintptr_t)right) {
		left->size += (right->size + mid->size);
		left->canary = (((uintptr_t)left)^CANARY_MAGIC_NUMBER) + 1;
		SETCAN(left);
		left->next_addr = right->next_addr;
		if (right->next_addr) right->next_addr->prev_addr = left;
		return 0;
	}
	return 1;

}

static bool mergeTwo(metadata_t *l, metadata_t *r) {
	if (l && r && (uintptr_t)l + l->size == (uintptr_t)r) {
		l->size += r->size;
		l->canary = (((uintptr_t)l)^CANARY_MAGIC_NUMBER) + 1;
		SETCAN(l);
		return 0;
	}
	return 1;
}



/* Set on every invocation of my_malloc()/my_free()/my_realloc()/
 * my_calloc() to indicate success or the type of failure. See
 * the definition of the my_malloc_err enum in my_malloc.h for details.
 * Similar to errno(3).
 */
enum my_malloc_err my_malloc_errno;

/* MALLOC
 * See my_malloc.h for documentation
 */
void *my_malloc(size_t size) {

	if (!size) {
		my_malloc_errno = NO_ERROR;
		return NULL;
	}
	size_t realSize = size + TOTAL_METADATA_SIZE;
	if (realSize > SBRK_SIZE) {
		my_malloc_errno = SINGLE_REQUEST_TOO_LARGE;
		return NULL;
	}

	metadata_t *i = size_list;
	for (; i; i = i->next_size) {
		if (i->size == realSize) {
			i->canary = (((uintptr_t)i)^CANARY_MAGIC_NUMBER) + 1;
			SETCAN(i);
			removeFromAlist(i);
			removeFromSizeList(i);
			my_malloc_errno = NO_ERROR; 
			return i+1;
		}
		if (i->size > realSize && i->size - realSize >= MIN_BLOCK_SIZE) {
			my_malloc_errno = NO_ERROR;
			return splitB(i, size);
		}

	}

	metadata_t *slice = my_sbrk(SBRK_SIZE);
	if (!slice) {
		my_malloc_errno = OUT_OF_MEMORY;
		return NULL;
	}
	slice->size = SBRK_SIZE - realSize;
	slice->canary = (((uintptr_t)slice)^CANARY_MAGIC_NUMBER) + 1;
	SETCAN(slice);
	metadata_t *n = (metadata_t *)((uint8_t*)(slice) + slice->size);
	n->size = realSize;
	n->canary = (((uintptr_t)n)^CANARY_MAGIC_NUMBER) + 1;
	SETCAN(n);
	if (!address_list) {
		address_list = slice;
		size_list = slice;
		slice->prev_addr = NULL;
		slice->next_addr = NULL;
		slice->prev_size = NULL;
		slice->next_size = NULL;
	} else {
		metadata_t *i = NULL;
		metadata_t *j = address_list;
		metadata_t *temp = NULL;
		for (; j && (uintptr_t)slice >= (uintptr_t)j; i=j,j=j->next_addr);
		if (doubleMergeAlist(i, slice, j)) {
			if (mergeTwo(i, slice)) {
				if (mergeTwo(slice, j)) {
					slice->next_addr = j;
					j->prev_addr = slice;
				} else {
					slice->next_addr = j->next_addr;
					if (j->next_addr) j->next_addr->prev_addr = slice;
					removeFromSizeList(j);
				}
				slice->prev_addr = i;
				if (i) i->next_addr = slice;
				temp = slice;

			} else {
				i->next_addr = j;
				if (j) j->prev_addr = i;
				removeFromSizeList(i);
				temp = i;
			}

		} else {
			removeFromSizeList(i);
			removeFromSizeList(j);
			removeFromAlist(j);
			temp = i;
		}
		i = NULL;
		j = size_list;
		for (; j && temp->size >= j->size; i = j, j = j->next_size);
		if (!i) {
			size_list = temp;
		} else {
			i->next_size = temp;
		}
		temp->prev_size = i;
		if (j) j->prev_size = temp;
		temp->next_size = j;
		
	}
	my_malloc_errno = NO_ERROR;
	return n + 1;

    //UNUSED_PARAMETER(size);
	//return (NULL);
}

/* REALLOC
 * See my_malloc.h for documentation
 */
void *my_realloc(void *ptr, size_t size) {

	if (!ptr) {
		return my_malloc(size);
	}
	unsigned long x = (((uintptr_t)((metadata_t*)ptr - 1))^CANARY_MAGIC_NUMBER) + 1;
	if (((metadata_t *)ptr - 1)->canary != x || GETCAN((metadata_t*)ptr - 1) !=((metadata_t*)ptr - 1)->canary) {
		my_malloc_errno = CANARY_CORRUPTED;
		return NULL;
	}
	if (!size) {
		my_free(ptr);
		return NULL;
	}

	return NULL;
}

/* CALLOC
 * See my_malloc.h for documentation
 */
void *my_calloc(size_t nmemb, size_t size) {
	void * temp = 0;
	if ((temp = my_malloc(nmemb * size))) {
		memset(temp, 0, nmemb * size);
		return temp;
	}

	return NULL;
}

/* FREE
 * See my_malloc.h for documentation
 */
void my_free(void *ptr) {
	if (!ptr) {
		my_malloc_errno = NO_ERROR;
		return;
	}
	metadata_t * meta = (metadata_t*)ptr - 1;
	unsigned long x = (((uintptr_t)((metadata_t*)meta - 1))^CANARY_MAGIC_NUMBER) + 1;
	if (meta->canary != x || meta->canary != GETCAN(meta)) {
		my_malloc_errno = CANARY_CORRUPTED;
		return;
	}
	if (!address_list) {
		address_list = meta;
		size_list = meta;
		meta->prev_addr = NULL;
		meta->next_addr = NULL;
		meta->prev_size = NULL;
		meta->next_size = NULL;
	} else {
		metadata_t *i = NULL;
		metadata_t *j = address_list;
		metadata_t *temp = NULL;
		for (; j && (uintptr_t)meta >= (uintptr_t)j; i=j,j=j->next_addr);
		if (doubleMergeAlist(i, meta, j)) {
			if (mergeTwo(i, meta)) {
				if (mergeTwo(meta, j)) {
					meta->next_addr = j;
					j->prev_addr = meta;
				} else {
					meta->next_addr = j->next_addr;
					if (j->next_addr) j->next_addr->prev_addr = meta;
					removeFromSizeList(j);
				}
				meta->prev_addr = i;
				if (i) i->next_addr = meta;
				temp = meta;

			} else {
				i->next_addr = j;
				if (j) j->prev_addr = i;
				removeFromSizeList(i);
				temp = i;
			}

		} else {
			removeFromSizeList(i);
			removeFromSizeList(j);
			removeFromAlist(j);
			temp = i;
		}
		if (!i) address_list = temp;
		i = NULL;
		j = size_list;
		for (; j && temp->size >= j->size; i = j, j = j->next_size);
		if (!i) {
			size_list = temp;
		} else {
			i->next_size = temp;
		}
		temp->prev_size = i;
		if (j) j->prev_size = temp;
		temp->next_size = j;
	}
	my_malloc_errno = NO_ERROR;



	//UNUSED_PARAMETER(ptr);
}
