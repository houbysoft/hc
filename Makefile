# Makefile for HC (HoubySoft Calculator)
# HC is (c) Jan Dlabal, 2009.
# HC is provided under the GNU GPLv3 License

# additional options to pass to gcc when compiling
#COPT=-DDBG -g -lefence -Wall
COPT=-Wall -g
#COPT=-DI_HATE_TIPS -Wall


# additional options to pass to gcc when linking
LOPT=

hc: main.o hc.o hc_functions.o hc_varassign.o hc_graph.o
	gcc `pkg-config --cflags --libs plplotd` ${LOPT} main.o hc.o hc_functions.o hc_varassign.o hc_graph.o /usr/lib/libmapm.a -lgdc -lgd -lm -lreadline -o $@

main.o: main.c
	gcc ${COPT} -c main.c

hc.o: hc.c
	gcc ${COPT} -c hc.c

hc_functions.o: hc_functions.c
	gcc ${COPT} -c hc_functions.c

hc_varassign.o: hc_varassign.c
	gcc ${COPT} -c hc_varassign.c

hc_graph.o: hc_graph.c
	gcc `pkg-config --cflags --libs plplotd` ${COPT} -c hc_graph.c

clean:
	rm hc&
	rm *.o&
	rm *~&
	rm *.png&