/**
 * sort.c
 * CS 2110 Function pointers lab
 * Name: YOUR NAME HERE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define UNUSED(x) (void)(x)

int verify_age_sort(void);
int verify_name_sort(void);
int verify_length_sort(void);
void shuffle(void);

typedef struct {
    int age;
    const char *name;
} person;

#define NUMPEOPLE 10
person people[NUMPEOPLE] = {
    {32, "Jim"      },
    {51, "Austin"   },
    {22, "Maddie"   },
    {25, "Preston"  },
    {37, "Daniel"   },
    {73, "Jered"    },
    {26, "Lauren"   },
    {40, "Cem"      },
    {68, "Jim"      },
    {19, "Farzam"   }
};

// TODO write any additional comparator functions here above main.
// We even gave you the beginning of one to start!
int compare_by_age(const void *data1, const void *data2) {
	int a = ((person *)data1)->age;
	int b = ((person *)data2)->age;
    //(data1);
    //UNUSED(data2);
    return a - b;
}

int compareWithName(const void *data1, const void *data2) {
    const char* a = ((person *)data1)->name;
    const char* b = ((person *)data2)->name;
    return strcmp(a, b);	
}

int compareWithLength(const void *data1, const void *data2) {
    const char* a = ((person *)data1)->name;
    const char* b = ((person *)data2)->name;
    
    int result = strlen(a) - strlen(b);	
    if (result != 0) {

	return result;
    } else {

	return strcmp(a, b);
    }
}

int main(void) {
    // Sets up some random stuff ;)
    srand(time(NULL));
    shuffle();

    // TODO Use qsort to sort people by age in ascending order
    qsort(people, NUMPEOPLE, sizeof(person), compare_by_age);


    // Print people sorted by age
    printf("----------------[Age sort]----------------\n");
    for(int i = 0; i < NUMPEOPLE; ++i) {
        printf("\t%-10s %-2d\n", people[i].name, people[i].age);
    }
    assert(verify_age_sort());
    shuffle();

    // TODO Use qsort to sort people alphabetically by name
    
    qsort(people, NUMPEOPLE, sizeof(person), compareWithName);

    // Print people sorted by name
    printf("---------[Name alphabetical sort]---------\n");
    for(int i = 0; i < NUMPEOPLE; ++i) {
        printf("\t%-10s %-2d\n", people[i].name, people[i].age);
    }
    assert(verify_name_sort());
    shuffle();

    // TODO Use qsort to sort people by the length of their name, ascending
    // If two names are the same length, alphabetically sort the two
    qsort(people, NUMPEOPLE, sizeof(person), compareWithLength);


    // Print people sorted by name length
    printf("-----------[Name length sort]-------------\n");
    for(int i = 0; i < NUMPEOPLE; ++i) {
        printf("\t%-10s %-2d\n", people[i].name, people[i].age);
    }
    assert(verify_length_sort());

    printf("Passed the tests!\n");
    return 0;
}

// Verification functions. Do not modify!

int verify_age_sort(void) {
    for(int i = 1; i < NUMPEOPLE; ++i) {
        if(people[i - 1].age > people[i].age) {
            return 0;
        }
    }
    return 1;
}

int verify_name_sort(void) {
    for(int i = 1; i < NUMPEOPLE; ++i) {
        if(strcmp(people[i - 1].name, people[i].name) > 0) {
            return 0;
        }
    }
    return 1;
}

int verify_length_sort(void) {
    for(int i = 1; i < NUMPEOPLE; ++i) {
        int diff = strlen(people[i - 1].name) - strlen(people[i].name);
        if(diff > 0) {
            return 0;
        } else if(!diff) {
            if(strcmp(people[i - 1].name, people[i].name) > 0) {
                return 0;
            }
        }
    }
    return 1;
}

void shuffle(void) {
    for(int i = NUMPEOPLE - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        person temp;
        memcpy(&temp, people + i, sizeof(person));
        memcpy(people + i, people + j, sizeof(person));
        memcpy(people + j, &temp, sizeof(person));
    }
}
