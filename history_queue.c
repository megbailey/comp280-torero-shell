/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "history_queue.h"

// global variables: add only globals for history list state
//                   all other variables should be allocated on the stack
// static: means these variables are only in scope in this .c module
static HistoryEntry history[MAXHIST];
static int queue_start = 0;
static int queue_next = 0;
static int queue_size = 0;

// Look at your lab 10 implementation of a circular queue of integers and
// reuse liberally.

/*
 * Adds a given element to history queue
 *
 * @param e Entry to be put into queue
 */
void add(HistoryEntry x) {
	// Until queue is full, increment next
	if (queue_next < MAXHIST) {
		history[queue_next] = x;
		queue_next++;
	} else {
		// Reset next if queue is full
		history[0] = x;
		queue_next = 1;
	}

	// Upping size until it is full
	if (queue_size < MAXHIST) {
		queue_size++;
	} else {
		// Once it's full we begin to move start
		if (queue_start < MAXHIST) {
			queue_start++;
		} else {
			queue_start = 0;
		}
	}
}

/*
 * Prints history queue in order
 */
void print() {
	int i = 0;
	int j = 0;
	while (i < queue_size){
		if(queue_start + i < MAXHIST) {
			printf("%d\t%s", history[queue_start + i].cmd_num, history[queue_start + i].cmdline);
		} else {
			printf("%d\t%s", history[j].cmd_num, history[j].cmdline);
			j++;
		}
		i++;
	}	
}

/*
 * Finds entry in history queue with specified id
 *
 * @param num Command id we want to lookup
 *
 * @return HistoryEntry containing the correct id
 *         if id isnt found in list we reutrn max unsigned int
 */
HistoryEntry search(unsigned int n) {
	HistoryEntry dummy;
	dummy.cmd_num = 4294967295;
	strcpy(dummy.cmdline, "DUMBO");
	int i;
	for(i = 0; i < queue_size; i++){
		if(n == history[i].cmd_num) {
			return history[i];
		}
	}
	return dummy;
}
