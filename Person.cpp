/*
 *  Person.cpp
 *
 *  Copyright (C) 2012  Alex Garcia <rotsmi@gmail.com>
 *
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <sys/time.h>
#include "Person.hpp"

/*  
 *
 * Implementing abstract, base and derived classes and function
 * using structs, dynamic memory allocation and function pointers
 * 
*/


Under *
Under_new(const char *name, char gender, const char *major, unsigned int year)
{
	// allocating memory space for a undergraduate "object"
	Under *uptr = (Under *) malloc(sizeof(Under));
	uptr->stud.per.name = strdup(name);

	// undergraduate student type = 2;
	uptr->stud.per.type = 2;

	// gender is a char type, so no need to allocate memory	
	uptr->stud.per.gender = gender;
	uptr->stud.per.work = &Student_work;
	uptr->stud.per.delet = &Under_delete;
	uptr->stud.graduate = &Under_graduate;
	uptr->stud.major = strdup(major);
	uptr->year = year;
	return uptr;
}

Grad *
Grad_new(const char *name, char gender, const char *major, const char *degree)
{
	// allocating memory space for a graduate "object"
	Grad *gptr = (Grad *) malloc(sizeof(Grad));

	// copy name to string s1, len1 consider the \0 char at the end of the string
	gptr->stud.per.name = strdup(name);

	// graduate student definde as type = 3;
	gptr->stud.per.type = 3;

	// char gender is not allocated on the heap, so we dont need to allocate memory	
	gptr->stud.per.gender = gender;
	gptr->stud.per.work = &Student_work;
	gptr->stud.per.delet = &Grad_delete;
	gptr->stud.graduate = &Grad_graduate;
	gptr->stud.major = strdup(major);
	gptr->degree = strdup(degree);

	return gptr;
}

  
Professor *
Professor_new(const char *name, char gender, const char *office)
{
	// Allocating memory spacer for a Professor Object
	Professor *pptr = (Professor *) malloc(sizeof(Professor));
	
	pptr->per.name = strdup(name);
	// professor definde as type = 1;
	pptr->per.type = 1;

	// char gender is not allocated on the heap, so we dont need to allocate memory	
	pptr->per.gender = gender;
	pptr->per.work = &Professor_work;
	pptr->per.delet = &Professor_delete;
	pptr->research = &Professor_research;

	// copy office to string p2, len2 consider the \0 char at the end of the string
	pptr->office = strdup(office);
	return pptr;
}  

void
Under_delete(Person *p)
{
	free((void *)p->name);	
	p->name = NULL;
	free((void *)((Student *)p)->major);
	((Student *)p)->major = NULL;
	free((void *)p);
	p = NULL;
}

void
Under_graduate(Student *s)
{
	printf("%s graduates in %d year and find a job"
		" in %s or goes to grad school.\n", s->per.name, 5 - ((Under *)s)->year, s->major); 
}

void
Student_work(Person *p)
{
	printf("%s studies %s\n", p->name, ((Student *)p)->major); 
}

void
Grad_delete(Person *p)
{
	free((void *)p->name);	
	p->name = NULL;
	free((void *)((Student *)p)->major);
	((Student *)p)->major = NULL;
	free((void *)((Grad *)p)->degree);
	((Grad *)p)->degree = NULL;
	free((void *)p);
	p = NULL;
}

void
Grad_graduate(Student *s)
{
	printf("%s graduates with a %s and find a job"
		" in %s .\n", s->per.name, ((Grad *)s)->degree, s->major); 
}

void
Professor_work(Person *p)
{
	printf("%s teaches.\n", p->name);
}

void
Professor_delete(Person *p)
{
	free((void *)p->name);	
	p->name = NULL;
	free((void *)((Professor *)p)->office);
	((Professor *)p)->office = NULL;
	free((void *)p);
	p = NULL;
}

void
Professor_research(Professor *pro, const char *topic)
{
	printf("%s does research in %s \n",((Person *)pro)->name, topic);
}


Student *
Person_downcast_Student(Person *p)
{
	Student *stud = NULL;
	if (p->type == 2 || p->type == 3)
		stud = (Student *)p;
	return stud;
}

Under *
Person_downcast_Under(Person *p)
{
	Under *und = NULL;
	if (p->type == 2)
		und = (Under *)p;
	return und;
}

Under *
Student_downcast_Under(Student *s)
{
	Under *und = NULL;
	if (s->per.type == 2)
		und = (Under *)s;
	return und;
}


Grad *
Person_downcast_Grad(Person *p)
{
	Grad *grd = NULL;
	if (p->type == 3)
		grd = (Grad *)p;
	return grd;
}


Grad *
Student_downcast_Grad(Student *s)
{
	Grad *grd = NULL;
	if (s->per.type == 3)
		grd = (Grad *)s;
	return grd;
}

Professor *
Person_downcast_Professor(Person *p)
{
	Professor *prof = NULL;
	if (p->type == 1)
		prof = (Professor *)p;
	return prof;
}

