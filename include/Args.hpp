#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <vector>

struct Args {
  Args(): socketName(""), mode(-1) {};
  std::vector<std::string> clientSocketsNames;
  std::string socketName;
  int mode;
};
