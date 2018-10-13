#include "cli_args.hpp"

#include "utils.hpp"

#include <iostream>

#define BOOST_TEST_MODULE Arguments

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

/*
 * Test that the arguments are interpreted right with the default seed
 * value of 1.
 */
BOOST_AUTO_TEST_CASE(cli_args_test_1)
{
  const char *argv[] = {"",
                        "--net", "filename",
                        "--units", "50",
                        "--ol", "1",
                        "--mht", "2",
                        "--mnu", "5",
                        "--st", "first",
                        "--mlc", "2",
                        "--population", "blablabla"};

  int argc = sizeof(argv) / sizeof(char *);

  cli_args args = process_cli_args(argc, argv);

  routing::set_st(args.st);

  BOOST_CHECK(args.population == "blablabla");

  BOOST_CHECK(args.net == "filename");
  BOOST_CHECK(args.units == 50);
  BOOST_CHECK(args.mlc.value() == 2);
  BOOST_CHECK(routing::get_st() == routing::st_t::first);
  
  BOOST_CHECK_CLOSE(args.ol, 1, 0.0001);
  BOOST_CHECK_CLOSE(args.mht, 2, 0.0001);
  BOOST_CHECK_CLOSE(args.mnu, 5.0, 0.0001);

  BOOST_CHECK(args.seed == 1);
}

/*
 * Make sure that:
 * - seed is 2
 * - st is fittest
 * - ml is 1000
 */
BOOST_AUTO_TEST_CASE(cli_args_test_2)
{
  const char *argv[] = {"",
                        "--net", "filename",
                        "--units", "50",
                        "--ol", "1",
                        "--mht", "2",
                        "--mnu", "5",
                        "--st", "fittest",
                        "--population", "blablabla",
                        "--seed", "2"};

  int argc = sizeof(argv) / sizeof(char *);

  cli_args args = process_cli_args(argc, argv);
  routing::set_st(args.st);

  BOOST_CHECK(args.seed == 2);
  BOOST_CHECK(routing::get_st() == routing::st_t::fittest);
}
