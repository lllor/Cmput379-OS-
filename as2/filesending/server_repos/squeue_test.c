/*CMPUT 201 Assignment License
 * Version 1.0
 *
 * Copyright 2018 Sarah Nadi
 
 * Unauthorized redistribution is forbidden under all circumstances. Use of this
 * software without explicit authorization from the author **and** the CMPUT 201
 * Instructor is prohibited.
 
 * This software was produced as part of an assignment in the course
 * CMPUT 201 - Practical Programming Methodology at the University of
 * Alberta, Canada. This code is confidential and remains confidential
 * after it is submitted for grading. The course staff has the right to
 * run plagiarism-detection tools on any code developed under this license,
 * even beyond the duration of the course.
 *
 * Copying any part of this code without including this copyright notice
 * is illegal.
 *
 * If any portion of this software is included in a solution submitted for
 * grading at an educational institution, the submitter will be subject to
 * the plagiarism sanctions at that institution.
 *
 * This software cannot be publicly posted under any circumstances, whether by
 * the original student or by a third party.
 * If this software is found in any public website or public repository, the
 * person finding it is kindly requested to immediately report, including
 * the URL or other repository locating information, to the following email
 * address:
 *
 *          nadi@ualberta.ca
 ***/
#include "squeue.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
  Squeue *s1 = NULL;
  initSqueue(&s1);
  //Squeue should be empty
  assert(isEmpty(s1));
  addBack(s1, "filename","content","md5");
  //printf("%s",s1->first->val);
  //We should have "Hello", "World!" in the Squeue in that order.
  
  //assert(peekFront(s1) == 'w');
  
  //addBack(s1, 'q')
  //assert(peekFront(s1)=='w');
  //assert(strcmp(peekBack(s1), 'w') == 0);
  return 0;
}
