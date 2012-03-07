/*
 *  Person.hpp
 *
 *  Copyright (C) 2012  Alex Garcia <rotsmi@gmail.com>
 *
 */

#ifndef _PERSON_HPP
#define _PERSON_HPP

/*   
 *  A Person behave as an abstract base class 
 *  Types for Persons: Professor(1), Under(2), Grad(3)
 *
*/
struct Person
{
	const char		*name;				/* string of any lengh */
	unsigned int	type;				/* 1, 2 or 3 */
	char			gender;				/* 'F' or 'M' */
	void			(*work)(Person *);	/* function pointer for work   */
	void			(*delet)(Person *);	/* function pointer for delete */
};

/*   
 *  A Student type that derived from a Person class. 
 *  Note that Person class is defined as the first element 
 *	in the Student class. The C standard guarantees that the 
 *	first member of a struct has the same address as the struct.
 *  We can then refer to any member of the struct Person from 
 *  Student allowing then to create the hierarchy.
 *
*/
struct Student
{
	Person			per;				/* base class person */
	const char		*major;				/* string major */
	void			(*graduate)(Student *); /*function poiter for graduate */
};

/*   
 *  An Under type class derived from Student class 
 *  As in student class refering Person as its base class, 
 *  we can refer from Under class to any member of the 
 *  struct Student allowing then to create the hierarchy.
 *
*/
struct Under
{
	Student			stud;				/* base class student */
	unsigned int	year;				/* the current year in the progra*/
};

/*   
 *  A Grad type class derived from Student class 
 *  Similar to Under class sharing the same Student base class, 
 *
*/
struct Grad
{
	Student			stud;				/* base class student */
	const char		*degree;			/* the degree program enrolled */
};

/*   
 *  A Professor type class derived directly from Person class 
 *  Similar to Student class sharing the same Person base class, 
 *
*/
struct Professor 
{
	Person			per;				/* base class person */
	const char		*office;			/* office where he/she works */
	void 			(*research) (Professor *p, const char *topic); /* function
											pointer to refer the method research */
};


/*  function that instantiate an undergraduate class (create an object under) */
Under *Under_new(const char *name, char gender, 
                 const char *major, unsigned int year);

/*  function that instantiate a graduate class (create an object grad) */
Grad *Grad_new(const char *name, char gender, 
               const char *major, const char *degree);  

/*  function that instantiate a professor class (create an object professor) */
Professor *Professor_new(const char *name, char gender, 
						 const char *office);  

/*  function that allow the downcasting from Person base class to derive classes */
Student *Person_downcast_Student(Person *);
Professor *Person_downcast_Professor(Person *);
Under *Person_downcast_Under(Person *);
Grad *Person_downcast_Grad(Person *);

/*  function that allow the downcasting from Student base class to derive classes */
Under *Student_downcast_Under(Student *);
Grad *Student_downcast_Grad(Student *);

/*  method that prints "<name> studies <major>" for Grad and Under. Since it print 
 *  the same thing for both the implementation can be share for Grad and Under. */
void Student_work(Person *p);

/*  method that delete an under object */
void Under_delete(Person *p);

/*  method that prints "<name> graduates in <N> years and finds a job in <major>"
	or goest to grad school. */
void Under_graduate(Student *s);

/*  method that delete a grad object */
void Grad_delete(Person *p);

/*  method that prints "<name> graduates with a <degree> and finds a job in <major>." */
void Grad_graduate(Student *s);

/*  method that prints "<name> teaches." */
void Professor_work(Person *p);

/*  method that delete an person object */
void Professor_delete(Person *p);

/*	method that prints "<name> does research in <topic>." */
void Professor_research(Professor *pro, const char *topic);


#endif /* _PERSON_HPP */
