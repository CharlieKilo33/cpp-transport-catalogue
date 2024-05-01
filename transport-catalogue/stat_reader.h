#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

using namespace transport;

void ReadStatRequests(const TransportCatalogue& transport_catalogue,
                      std::istream& input, std::ostream& output);

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue,
                       std::string_view request, std::ostream& output);