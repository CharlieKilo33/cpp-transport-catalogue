#pragma once

#include <cmath>

namespace geo {
struct Coordinates {
  Coordinates() = default;
  Coordinates(double _lat, double _lng);
  double lat;
  double lng;
  bool operator==(const Coordinates &other) const {
    return lat == other.lat && lng == other.lng;
  }
  bool operator!=(const Coordinates &other) const { return !(*this == other); }
};

double ComputeDistance(Coordinates from, Coordinates to);
}  // namespace geo