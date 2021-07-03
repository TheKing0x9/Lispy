ifeq ($(origin CC),default)
CC = gcc
endif

lispy: lispy.c ltypes.c mpc.c lispy.h mpc.h
	make clean
	$(CC) lispy.c mpc.c ltypes.c -o lispy -Wall -Wextra -pedantic -std=c99
	
clean:
	del lispy.exe
