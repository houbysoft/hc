# Makefile for HC (HoubySoft Calculator)
# HC is (c) Jan Dlabal, 2009-2010.
# HC is provided under the GNU GPLv3 License

# additional options to pass to gcc when compiling
COPT=-DDBG -g -lefence -Wall
#COPT=-Wall
#COPT=-DI_HATE_TIPS -Wall


# additional options to pass to gcc when linking
LOPT=

hc: main.o hc.o hc_functions.o hc_varassign.o hc_graph.o hc_complex.o m_apmc.o hc_stats.o hc_programming.o hc_chemistry.o hc_list.o
	gcc `pkg-config --cflags --libs plplotd` ${LOPT} main.o hc.o hc_functions.o hc_varassign.o hc_graph.o hc_complex.o m_apmc.o hc_stats.o hc_programming.o hc_chemistry.o hc_list.o /usr/lib/libmapm.a -lgdc -lgd -lm -lreadline -o $@

m_apmc.o: m_apmc.c
	gcc ${COPT} -c m_apmc.c

main.o: main.c
	gcc ${COPT} -c main.c

hc_stats.o: hc_stats.c
	gcc ${COPT} -c hc_stats.c

hc_list.o: hc_list.c
	gcc ${COPT} -c hc_list.c

hc.o: hc.c
	gcc ${COPT} -c hc.c

hc_functions.o: hc_functions.c
	gcc ${COPT} -c hc_functions.c

hc_varassign.o: hc_varassign.c
	gcc ${COPT} -c hc_varassign.c

hc_programming.o: hc_programming.c
	gcc ${COPT} -c hc_programming.c

hc_chemistry.o: hc_chemistry.c
	gcc ${COPT} -c hc_chemistry.c

hc_graph.o: hc_graph.c
	gcc `pkg-config --cflags --libs plplotd` ${COPT} -c hc_graph.c

hc_complex.o: hc_complex.c
	gcc ${COPT} -c hc_complex.c


