/*
 * lab1a.c
 *
 *  Created on:
 *      Author:
 */

/* include helper functions for game */
#include "LIFE.h"

/* add whatever other includes here */

/* number of generations to evolve the world */
#define NUM_GENERATIONS 50

/* functions to implement */

/* this function should set the state of all
   the cells in the next generation and call
   finalize_evolution() to update the current
   state of the world to the next generation */
void next_generation(void);

/* this function should return the state of the cell
   at (x,y) in the next generation, according to the
   rules of Conway's Game of Life (see handout) */
int get_next_state(int x, int y);

/* this function should calculate the number of alive
   neighbors of the cell at (x,y) */
int num_neighbors(int x, int y);

int main(void)
{
	int n;
	char filename[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB_1\\sship.txt";
	char file_out[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB_1\\lab1_out.txt";
	/* TODO: initialize the world */
	//initialize_world();
	initialize_world_from_file(filename);
	for (n = 0; n < NUM_GENERATIONS; n++)
	{
		next_generation();

	}
	/* TODO: output final world state */
	save_world_to_file(file_out);
	return 0;
}

void next_generation(void) {
	/* TODO: for every cell, set the state in the next
	   generation according to the Game of Life rules

	   Hint: use get_next_state(x,y) */
	int Width, Height;
	for(Height = get_world_height() - 1; Height>-1;Height--)
	{
		for(Width = get_world_width() - 1; Width>-1;Width--)
		{
			set_cell_state(Width,Height,get_next_state(Width,Height));
		}
	}
	finalize_evolution(); /* called at end to finalize */
}

int get_next_state(int x, int y) {
	/* TODO: for the specified cell, compute the state in
	   the next generation using the rules

	   Use num_neighbors(x,y) to compute the number of live
	   neighbors */
	int Live_Neighbors, Curr_State;
	Live_Neighbors = num_neighbors(x,y);
	Curr_State = get_cell_state(x,y);
	if(Curr_State)
	{
		if((Live_Neighbors-1) < 2)
			return DEAD;
		else if ((Live_Neighbors-1) < 4)
			return ALIVE;
		else
			return DEAD;
	}
	else
	{
		if(Live_Neighbors == 3)
			return ALIVE;
		else
			return DEAD;
	}
}

int num_neighbors(int x, int y) {
	/* TODO: for the specified cell, return the number of
	   neighbors that are ALIVE

	   Use get_cell_state(x,y) */
	int i,j,count = 0;
	for(i = -1; i<2; i++)
	{
		for(j = -1; j<2; j++)
		{
			if (get_cell_state((x+i),(y+j)))
				count++;
		}
	}
	return count;
}
