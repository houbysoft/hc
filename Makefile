# Makefile for HC (HoubySoft Calculator)
# HC is (c) Jan Dlabal, 2009-2010.
# HC is provided under the GNU GPLv3 License

# additional options to pass to gcc when compiling
# -DDDBG for even more debug messages
#COPT=-DDBG -g -lefence -Wall
COPT=-Wall
#COPT=-DI_HATE_TIPS -Wall


# additional options to pass to gcc when linking
LOPT=

OBJS=main.o hc.o hc_functions.o hc_varassign.o hc_graph.o hc_complex.o m_apmc.o hc_stats.o hc_programming.o hc_chemistry.o hc_list.o hc_help.o hc_utils.o

hc: cleanup ${OBJS}
	gcc `pkg-config --cflags --libs plplotd` ${LOPT} ${OBJS} /usr/lib/libmapm.so.0 -lgd -lm -lreadline -o $@

cleanup: hc.make.cli

hc.make.cli:
	rm -f *.o
	rm -f hc.make.gui
	touch hc.make.cli

hc_graph.o: hc_graph.c
	gcc `pkg-config --cflags --libs plplotd` ${COPT} -c hc_graph.c

%.o: %.c hc.h
	gcc ${COPT} -c $<
