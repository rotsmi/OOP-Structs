PROJECT = prog1
SRC_FILES = \
	main.cpp \
	Person.hpp \
	Person.cpp \
	List.hpp \
	Makefile \
	README
a.out:
	g++ -g Person.cpp main.cpp -o a.out

#Removes all objects and executables.
clean:
	rm -f  *.out *.o  core *.core *~

#This target makes a archive containing all the project source files,
#suitable for submission.
submit:	$(SRC_FILES)
	tar cvfz $(PROJECT).tar.gz $(SRC_FILES)
