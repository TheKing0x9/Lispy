 ifeq ($(origin CC),default)
CC = gcc
endif

lispy: lispy.c ltypes.c mpc.c lispy.h mpc.h builtins.c
	make clean
	$(CC) lispy.c mpc.c ltypes.c builtins.c -o lispy -Wall -Wextra -pedantic -std=c99
	
clean:
	del lispy.exe
