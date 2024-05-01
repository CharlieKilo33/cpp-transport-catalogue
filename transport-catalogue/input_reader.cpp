#include "input_reader.h"

#include <algorithm>
#include <iterator>
#include <sstream>

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта,
 * долгота)
 */
Coordinates ParseCoordinates(std::string_view str) {
  static const double nan = std::nan("");

  auto not_space = str.find_first_not_of(' ');
  auto comma = str.find(',');

  if (comma == str.npos) {
    return {nan, nan};
  }

  auto not_space2 = str.find_first_not_of(' ', comma + 1);

  double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
  double lng = std::stod(std::string(str.substr(not_space2)));

  return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
  const auto start = string.find_first_not_of(' ');
  if (start == string.npos) {
    return {};
  }
  return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя
 * delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
  std::vector<std::string_view> result;

  size_t pos = 0;
  while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
    auto delim_pos = string.find(delim, pos);
    if (delim_pos == string.npos) {
      delim_pos = string.size();
    }
    if (auto substr = Trim(string.substr(pos, delim_pos - pos));
        !substr.empty()) {
      result.push_back(substr);
    }
    pos = delim_pos + 1;
  }

  return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок
 * [A,B,C,A] Для некольцевого маршрута (A-B-C-D) возвращает массив названий
 * остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
  if (route.find('>') != route.npos) {
    return Split(route, '>');
  }

  auto stops = Split(route, '-');
  std::vector<std::string_view> results(stops.begin(), stops.end());
  results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

  return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
  auto colon_pos = line.find(':');
  if (colon_pos == line.npos) {
    return {};
  }

  auto space_pos = line.find(' ');
  if (space_pos >= colon_pos) {
    return {};
  }

  auto not_space = line.find_first_not_of(' ', space_pos);
  if (not_space >= colon_pos) {
    return {};
  }

  return {std::string(line.substr(0, space_pos)),
          std::string(line.substr(not_space, colon_pos - not_space)),
          std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
  auto command_description = ParseCommandDescription(line);
  if (command_description) {
    commands_.push_back(std::move(command_description));
  }
}

struct Distance {
  std::string stop_name_;
  double distance_ = 0;
};

std::vector<Distance> ParseDistances(std::string_view str) {
  std::istringstream iss(str.data());
  std::vector<Distance> res;
  std::string units, to, stop_name;
  double distance;
  iss.ignore(256, ',');
  iss.ignore(256, ',');

  while (iss) {
    if (!(iss >> distance)) break;
    iss >> units;
    iss >> to;
    std::getline(iss, stop_name, ',');
    stop_name.erase(0, stop_name.find_first_not_of(" \t\r\n"));
    Distance new_dist;
    new_dist.distance_ = distance;
    new_dist.stop_name_ = stop_name;
    res.emplace_back(new_dist);
  }
  return res;
}

void InputReader::ApplyCommandStop(transport::TransportCatalogue& catalogue,
                                   const CommandDescription& command) {
  Stop temp_stop;
  temp_stop.name = command.id;
  temp_stop.coordinates = ParseCoordinates(command.description);
  catalogue.AddStop(temp_stop);
}

void InputReader::ApplyCommandBus(
    TransportCatalogue& catalogue,
    const std::vector<CommandDescription>& busses) {
  for (const auto& bus : busses) {
    std::vector<detail::Stop*> temp_bus_stops;
    for (const auto stop : ParseRoute(bus.description)) {
      temp_bus_stops.push_back(catalogue.FindStop(stop));
    }
    Bus temp_bus;
    temp_bus.number = bus.id;
    temp_bus.stops = std::move(temp_bus_stops);
    catalogue.AddBus(temp_bus);
  }
}

void InputReader::ApplyCommands(
    [[maybe_unused]] TransportCatalogue& catalogue) const {
  std::vector<CommandDescription> busses;
  for (const auto& command : commands_) {
    if (command.command == "Stop") {
      ApplyCommandStop(catalogue, command);
    } else if (command.command == "Bus") {
      busses.push_back(command);
    } else {
      break;
    }
  }

  for (const auto& command : commands_) {
    if (command.command == "Stop") {
      std::vector<Distance> distances = ParseDistances(command.description);
      for (const auto& distance : distances) {
        catalogue.AddDistanceBetweenStops(command.id, distance.stop_name_,
                                          distance.distance_);
      }
    } else if (command.command == "Bus") {
      continue;
    } else {
      break;
    }
  }

  if (!busses.empty()) {
    ApplyCommandBus(catalogue, busses);
  }
}

void ReadBaseRequests(std::istream& input_stream, TransportCatalogue& catalogue,
                      InputReader& reader) {
  int base_request_count;
  input_stream >> base_request_count >> std::ws;
  for (int i = 0; i < base_request_count; ++i) {
    std::string line;
    getline(input_stream, line);
    reader.ParseLine(line);
  }
  reader.ApplyCommands(catalogue);
}