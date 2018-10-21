#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <vector>

struct Args {
  std::vector<std::string> clientSocketsNames;
  std::string socketName = "";
  int mode = -1;
  bool dumpProtocol = false;
};
