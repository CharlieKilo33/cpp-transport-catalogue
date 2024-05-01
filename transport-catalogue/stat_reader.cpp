#include "stat_reader.h"

#include <iomanip>

using namespace std::literals;

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue,
                       std::string_view request, std::ostream& output) {
  auto temp = request.find_first_of(' ');
  auto first_word = request.substr(0, temp);
  if (first_word == "Bus") {
    auto second_word = request.substr(temp + 1);
    output << request << ": "s;
    auto info = transport_catalogue.GetBusInfo(second_word);
    if (info.empty()) {
      output << "not found\n"s;
      return;
    } else {
      output << std::to_string(info.size()) << " stops on route, "s
             << std::to_string(
                    transport_catalogue.GetUniqueStations(second_word))
             << " unique stops, "s << std::setprecision(6)
             << transport_catalogue.FindBus(second_word)->length_between_bus_stops
             << " route length, "s << transport_catalogue.FindBus(second_word)->curvature << " curvature\n"s;
    }
  } else if (first_word == "Stop") {
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
}