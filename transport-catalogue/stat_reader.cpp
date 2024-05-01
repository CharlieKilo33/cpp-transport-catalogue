#include "stat_reader.h"

#include <iomanip>

using namespace std::literals;

void ReadStatRequests(const TransportCatalogue& catalogue, std::istream& input,
                      std::ostream& output) {
  int stat_request_count;
  input >> stat_request_count >> std::ws;
  for (int i = 0; i < stat_request_count; ++i) {
    std::string line;
    getline(input, line);
    ParseAndPrintStat(catalogue, line, output);
  }
}

void PrintBusStat(const TransportCatalogue& transport_catalogue,
                  std::string_view request, std::ostream& output) {
  auto temp = request.find_first_of(' ');
  auto second_word = request.substr(temp + 1);
  output << request << ": "s;
  auto info = transport_catalogue.GetBusInfo(second_word);
  if (info == nullptr) {
    output << "not found\n"s;
    return;
  } else {
    output << std::to_string(info->stops.size()) << " stops on route, "s
           << std::to_string(info->unique_stops) << " unique stops, "s
           << std::setprecision(6) << info->length_between_bus_stops
           << " route length, "s << info->curvature << " curvature\n"s;
  }
}

void PrintStopStat(const TransportCatalogue& transport_catalogue,
                   std::string_view request, std::ostream& output) {
  auto temp = request.find_first_of(' ');
  auto second_word = request.substr(temp + 1);
  output << request << ": "s;
  if (transport_catalogue.FindStop(second_word) == nullptr) {
    output << "not found\n"s;
    return;
  }
  auto info = transport_catalogue.GetBusesThroughStop(second_word);
  if (info.empty()) {
    output << "no buses\n"s;
    return;
  } else {
    output << "buses "s;
    for (auto bus : info) {
      output << bus << " ";
    }
    output << '\n';
  }
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue,
                       std::string_view request, std::ostream& output) {
  auto temp = request.find_first_of(' ');
  auto first_word = request.substr(0, temp);
  if (first_word == "Bus") {
    PrintBusStat(transport_catalogue, request, output);
  } else if (first_word == "Stop") {
    PrintStopStat(transport_catalogue, request, output);
  }
}