ifeq ($(origin CC),default)
CC = gcc
endif

lispy: lispy.c
	$(CC) lispy.c mpc.c ltypes.c -o lispy -Wall -Wextra -pedantic -std=c99
