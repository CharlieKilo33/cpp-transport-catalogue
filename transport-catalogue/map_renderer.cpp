#include "map_renderer.h"

bool IsZero(double value) { return std::abs(value) < EPSILON; }

MapRenderer::MapRenderer(RenderContext render_context)
    : render_context_(std::move(render_context)) {}

void MapRenderer::AddContext(RenderContext render_context) {
  render_context_ = std::move(render_context);
  std::sort(render_context_.buses_route.begin(),
            render_context_.buses_route.end(),
            [](detail::Bus *lhs, detail::Bus *rhs) {
              return lhs->number < rhs->number;
            });
}

std::vector<svg::Polyline> MapRenderer::GetRouteLines(
    const std::vector<detail::Bus *> &buses, const SphereProjector &sp) const {
  std::vector<svg::Polyline> result;
  size_t color_num = 0;
  for (const auto &bus : buses) {
    if (bus->stops.empty()) continue;
    std::vector<const detail::Stop *> route_stops{bus->stops.begin(),
                                                  bus->stops.end()};
    svg::Polyline line;
    for (const auto &stop : route_stops) {
      line.AddPoint(sp(stop->coordinates));
    }
    line.SetStrokeColor(
        render_context_.render_settings.color_palette[color_num]);
    line.SetFillColor("none");
    line.SetStrokeWidth(render_context_.render_settings.line_width);
    line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    if (color_num <
        (render_context_.render_settings.color_palette.size() - 1)) {
      ++color_num;
    } else {
      color_num = 0;
    }
    result.push_back(line);
  }

  return result;
}

std::vector<svg::Text> MapRenderer::GetBusLabel(
    const std::vector<detail::Bus *> &buses, const SphereProjector &sp) const {
  std::vector<svg::Text> result;
  size_t color_num = 0;
  for (const auto &bus : buses) {
    if (bus->stops.empty()) {
      continue;
    }
    svg::Text text;
    svg::Text underlayer;
    text.SetPosition(sp(bus->stops[0]->coordinates));
    text.SetOffset({render_context_.render_settings.bus_label_offset.first,
                    render_context_.render_settings.bus_label_offset.second});
    text.SetFontSize(render_context_.render_settings.bus_label_font_size);
    text.SetFontFamily("Verdana");
    text.SetFontWeight("bold");
    text.SetData(bus->number);
    text.SetFillColor(render_context_.render_settings.color_palette[color_num]);
    if (color_num <
        (render_context_.render_settings.color_palette.size() - 1)) {
      ++color_num;
    } else {
      color_num = 0;
    }

    underlayer.SetPosition(sp(bus->stops[0]->coordinates));
    underlayer.SetOffset(
        {render_context_.render_settings.bus_label_offset.first,
         render_context_.render_settings.bus_label_offset.second});
    underlayer.SetFontSize(render_context_.render_settings.bus_label_font_size);
    underlayer.SetFontFamily("Verdana");
    underlayer.SetFontWeight("bold");
    underlayer.SetData(bus->number);
    underlayer.SetFillColor(render_context_.render_settings.underlayer_color);
    underlayer.SetStrokeColor(render_context_.render_settings.underlayer_color);
    underlayer.SetStrokeWidth(render_context_.render_settings.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    result.push_back(underlayer);
    result.push_back(text);

    if (!bus->is_roundtrip &&
        bus->stops[0] != bus->stops[bus->stops.size() / 2]) {
      svg::Text text2{text};
      svg::Text underlayer2{underlayer};
      text2.SetPosition(sp(bus->stops[bus->stops.size() / 2]->coordinates));
      underlayer2.SetPosition(
          sp(bus->stops[bus->stops.size() / 2]->coordinates));

      result.push_back(underlayer2);
      result.push_back(text2);
    }
  }
  return result;
}

std::vector<svg::Circle> MapRenderer::GetStopsSymbols(
    const std::vector<detail::Stop *> &stops, const SphereProjector &sp) const {
  std::vector<svg::Circle> result;
  for (const auto &stop : stops) {
    svg::Circle symbol;
    symbol.SetCenter(sp(stop->coordinates));
    symbol.SetRadius(render_context_.render_settings.stop_radius);
    symbol.SetFillColor("white");

    result.push_back(symbol);
  }

  return result;
}

std::vector<svg::Text> MapRenderer::GetStopsLabels(
    const std::vector<detail::Stop *> &stops, const SphereProjector &sp) const {
  std::vector<svg::Text> result;
  svg::Text text;
  svg::Text underlayer;
  for (const auto &stop : stops) {
    text.SetPosition(sp(stop->coordinates));
    text.SetOffset({render_context_.render_settings.stop_label_offset.first,
                    render_context_.render_settings.stop_label_offset.second});
    text.SetFontSize(render_context_.render_settings.stop_label_font_size);
    text.SetFontFamily("Verdana");
    text.SetData(stop->name);
    text.SetFillColor("black");

    underlayer.SetPosition(sp(stop->coordinates));
    underlayer.SetOffset(
        {render_context_.render_settings.stop_label_offset.first,
         render_context_.render_settings.stop_label_offset.second});
    underlayer.SetFontSize(
        render_context_.render_settings.stop_label_font_size);
    underlayer.SetFontFamily("Verdana");
    underlayer.SetData(stop->name);
    underlayer.SetFillColor(render_context_.render_settings.underlayer_color);
    underlayer.SetStrokeColor(render_context_.render_settings.underlayer_color);
    underlayer.SetStrokeWidth(render_context_.render_settings.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    result.push_back(underlayer);
    result.push_back(text);
  }

  return result;
}

svg::Document MapRenderer::GetSVG() const {
  std::vector<detail::Bus *> buses = render_context_.buses_route;
  std::vector<detail::Stop *> stops;
  svg::Document result;
  std::vector<geo::Coordinates> route_stops_coord;

  for (const auto &bus : buses) {
    if (bus->stops.empty()) continue;
    for (auto i = 0; i < int(bus->stops.size()); ++i) {
      route_stops_coord.push_back(bus->stops[i]->coordinates);
      if (std::count(stops.begin(), stops.end(), bus->stops[i])) {
        continue;
      }
      if (bus->is_roundtrip) {
        if (i == int(bus->stops.size()) - 1) {
          break;
        }
        stops.push_back(bus->stops[i]);
      } else {
        if (i == int(bus->stops.size()) / 2) {
          stops.push_back(bus->stops[i]);
          break;
        } else {
          stops.push_back(bus->stops[i]);
        }
      }
    }
  }
  SphereProjector sp(route_stops_coord.begin(), route_stops_coord.end(),
                     render_context_.render_settings.width,
                     render_context_.render_settings.height,
                     render_context_.render_settings.padding);
  std::sort(stops.begin(), stops.end(), [](detail::Stop *lhs, detail::Stop *rhs) {
    return lhs->name < rhs->name;
  });

  for (const auto &line : GetRouteLines(buses, sp)) result.Add(line);
  for (const auto &text : GetBusLabel(buses, sp)) result.Add(text);
  for (const auto &circle : GetStopsSymbols(stops, sp)) result.Add(circle);
  for (const auto &text : GetStopsLabels(stops, sp)) result.Add(text);

  return result;
}