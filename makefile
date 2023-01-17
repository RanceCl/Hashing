# makefile for MP7
#
# -lm is used to link in the math library
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

lab7 : table_oa.o table_sc.o table.o lab7.o
	$(comp) $(comp_flags) table_oa.o table_sc.o table.o lab7.o -o lab7 $(comp_libs)

table_oa.o : table_oa.c table.h
	$(comp) $(comp_flags) -c table_oa.c

table_sc.o : table_sc.c table.h
	$(comp) $(comp_flags) -c table_sc.c

table.o : table.c table.h
	$(comp) $(comp_flags) -c table.c

lab7.o : lab7.c table.h
	$(comp) $(comp_flags) -c lab7.c

clean :
	rm -f *.o lab7 core

