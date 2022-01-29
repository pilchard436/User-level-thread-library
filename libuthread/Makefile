# Target library
lib := libuthread.a
objects = queue.o #preempt.o uthread.o context.o
all: $(lib)

## TODO

$(lib): %.o:
	ar rcs $(lib) $^ -l

queue.o: queue.c queue.h
	gcc -Wall -Werror -g $< -c

# preempt.o:
# uthread.o:
# context.o:

clean:
	rm -f $(lib) queue.o

#ar rcs libuthread.a file1.o file2.o #ar rcs my_library.a file1.o file2.o


#Sergio's research:

#1)reference link: https://stackoverflow.com/questions/31421616/c-creating-static-library-and-linking-using-a-makefile/31421842

#2) Reference Link(Flag options): https://www.gnu.org/software/make/manual/html_node/Options-Summary.html
# -c creates the object file: https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/#:~:text=c%20file%20using%20the%20compiler,
#macro%20is%20defined%20as%20above.

#3)Reference Link(Makefile tutorial): https://makefiletutorial.com/
#(CLTRL + F): Here's the more efficient way, using a static pattern rule:
# reference code
# 	objects = foo.o bar.o all.o
# 	all: $(objects)
# 	# These files compile via implicit rules
# 	# Syntax - targets ...: target-pattern: prereq-patterns ...
# 	# In the case of the first target, foo.o, the target-pattern matches foo.o and sets the "stem" to be "foo".
# 	# It then replaces the '%' in prereq-patterns with that stem
# 	$(objects): %.o: %.c

#4)reference link: https://stackoverflow.com/questions/3220277/what-do-the-makefile-symbols-and-mean
# reference code
# 	all: library.cpp main.cpp
# 	$@ evaluates to all
# 	$< evaluates to library.cpp
# 	$^ evaluates to library.cpp main.cpp


