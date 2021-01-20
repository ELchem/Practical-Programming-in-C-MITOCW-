/*
 * sqlprojLibrary.h
 *
 *  Created on: Mar 27, 2020
 *      Author: Dell
 */

#ifndef SQLPROJLIBRARY_H_
#define SQLPROJLIBRARY_H_

int initialize_db (const char * filename);
int locate_movie(const char * title);
void dump_sorted_list(const char * filename);
void cleanup(void);


#endif /* SQLPROJLIBRARY_H_ */
