#ifndef CLI_ARGS_HPP
#define CLI_ARGS_HPP

#include "connection.hpp"
#include "routing.hpp"

#include <optional>
#include <string>

/**
 * These are the program arguments.  In this single class we store all
 * information passed at the command line.
 */
struct cli_args
{
  /// -----------------------------------------------------------------
  /// The network options
  /// -----------------------------------------------------------------
  
  /// The network file name.
  std::string net;

  /// The number of units.
  int units;

  /// The maximal length of a path we want.
  std::optional<COST> ml;

  /// The maximal length coefficient of a path we want.
  std::optional<float> mlc;

  /// The K for the k-shortest paths.
  std::optional<unsigned> K;

  /// The spectrum selection type.
  std::string st;

  /// Use the parallel search.
  bool parallel = false;

  // Use the brute force search.
  bool brtforce = false;

  // Use the puyenksp search.
  bool puyenksp = false;

  /// -----------------------------------------------------------------
  /// The traffic options
  /// -----------------------------------------------------------------

  /// The mean client arrival time.
  double mcat;

  /// The offered load.
  double ol;

  /// The mean holding time.
  double mht;

  /// The mean number of units.
  double mnu;

  /// -----------------------------------------------------------------
  /// The simulation options
  /// -----------------------------------------------------------------

  /// The seed
  int seed;

  /// The population name.
  std::string population;

  /// The kickoff time for stats.
  double kickoff;

  /// The limit on the simulation time.
  double sim_time;
};

/**
 * This function parses the command-line arguments.
 */
cli_args
process_cli_args(int argc, const char* argv[]);

#endif /* CLI_ARGS_HPP */
