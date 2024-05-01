#include <iostream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
  TransportCatalogue catalogue;

  {
    InputReader reader;
    ReadBaseRequests(cin, catalogue, reader);
  }

  ReadStatRequests(catalogue, cin, cout);
}