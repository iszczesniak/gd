TESTS = adaptive_units cli_args dijkstra graph units utils various

OBJS = sample_graphs.o ../client.o ../cli_args.o ../connection.o	\
	../routing.o ../stats.o ../traffic.o ../utils.o

CXXFLAGS = -g -Wno-deprecated -std=c++17

CXXFLAGS := $(CXXFLAGS) -I ../

ifdef BOOST_ROOT
	CXXFLAGS := $(CXXFLAGS) -I $(BOOST_ROOT)/include
	LDFLAGS := $(LDFLAGS) -L $(BOOST_ROOT)/lib
endif

# Boost
LDFLAGS := $(LDFLAGS) -l boost_graph
LDFLAGS := $(LDFLAGS) -l boost_program_options
LDFLAGS := $(LDFLAGS) -l boost_system
LDFLAGS := $(LDFLAGS) -l boost_test_exec_monitor
LDFLAGS := $(LDFLAGS) -l boost_unit_test_framework

.PHONY: clean depend run

all: $(TESTS)

adaptive_units: adaptive_units.o $(OBJS)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

cli_args: cli_args.o $(OBJS)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

dijkstra: dijkstra.o $(OBJS)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

graph: graph.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

units: units.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

utils: utils.o $(OBJS)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

various: various.o $(OBJS)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# run the tests
run:
	@for i in $(TESTS); do echo "Running" $$i; ./$$i; done

count:
	wc -l *.hpp *.cc

clean:
	rm -rf *~
	rm -rf *.o
	rm -rf $(TESTS)

depend:
	g++ -MM $(CXXFLAGS) *.cc > dependencies

include dependencies
