#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью
 * предметной области (domain) вашего приложения и не зависят от транспортного
 * справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в
 * отдельный заголовочный файл может оказаться полезным, когда дело дойдёт до
 * визуализации карты маршрутов: визуализатор карты (map_renderer) можно будет
 * сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте
 * этот файл пустым.
 *
 */

#include <string>
#include <vector>

#include "geo.h"
namespace detail {
struct Stop {
  Stop() = default;
  std::string name;
  geo::Coordinates coordinates{};
  [[maybe_unused]] std::vector<std::pair<std::string, double>> distance;
};

struct Bus {
  Bus() = default;
  std::string number;
  std::vector<Stop *> stops;
  double curvature = 0;
  double length_between_bus_stops = 0;
  size_t unique_stops = 0;
  [[maybe_unused]] bool is_roundtrip = false;
  [[maybe_unused]] std::vector<std::string> stops_names;
};
}  // namespace detail