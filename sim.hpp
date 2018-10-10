#ifndef SIM_HPP
#define SIM_HPP

#include "simulation.hpp"
#include "graph.hpp"
#include "module.hpp"

#include <random>

typedef simulation<double, graph, std::default_random_engine> sim;
typedef module<sim> mod;

#endif /* SIM_HPP */
