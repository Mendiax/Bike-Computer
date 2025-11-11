#include "views/screenfunc/gps.h"
#include "common_types.h"
#include "display/driver.hpp"
#include "traces.h"
#include <algorithm>
#include <cstdio>
#include <utility>

using mapType = int_fast16_t;

template <typename T> struct Bounds {
  struct Axis {
    T min;
    T max;
  };
  Axis horizontal;
  Axis vertical;
};

struct MapPoint {
  mapType x;
  mapType y;
};

using MapBounds = Bounds<mapType>;

void render_gps_path(const void *settings);

DrawFuncT get_draw_func_gps_path() { return render_gps_path; }

template <typename T, typename O>
static inline O convert(const T &val, const T &min, const T &max,
                        const O &min_o, const O &max_o) {
  O val_o = static_cast<O>(
      (static_cast<double>(val - min) / static_cast<double>(max - min)) *
          static_cast<double>(max_o - min_o) +
      min_o);
  return val_o; // not tested
}

template <typename T, typename O>
static inline O map(const T &val, const T &min, const T &max, const O &min_o,
                    const O &max_o) {
  // std::cout << "val_o: " << val_o << " " << val << " " << min << " " << max
  // << " " << min_o << " " << max_o  << std::endl;
  //  test input
  if (min == max) {
    return max_o; // or max_o; since min == max, both represent the same point
  }
  if (val <= min) {
    return min_o;
  }
  if (val >= max) {
    return max_o;
  }
  O val_o = static_cast<O>(
      (static_cast<double>(val - min) / static_cast<double>(max - min)) *
          static_cast<double>(max_o - min_o) +
      min_o);

  // test output
  if (val_o <= min_o) {
    return min_o;
  }
  if (val_o >= max_o) {
    return max_o;
  }
  return val_o; // not tested
}

static inline bool is_in_bounds(const MapPoint &point,
                                const MapBounds &bounds) {
  return (point.x >= bounds.horizontal.min &&
          point.x <= bounds.horizontal.max && point.y >= bounds.vertical.min &&
          point.y <= bounds.vertical.max);
}

static inline void draw_line_between_points(MapPoint p0, MapPoint p1,
                                            const display::DisplayColor &color,
                                            const MapBounds &bounds) {
  TRACE_DEBUG(8, TRACE_VIEWS,
              "GPS Plot: Drawing line from (%ld, %ld) to (%ld, %ld)\n",
              (long)p0.x, (long)p0.y, (long)p1.x, (long)p1.y);

  // This is not 100% correct as some points even outside the bounds could be
  // drawn If one is obove on top and the other below on bottom, the line should
  // be drawn But this is a rare case and for simplicity we will just skip
  // drawing such lines In future we could implement Cohen-Sutherland or
  // Liang-Barsky line clipping algorithms
  const bool p0_in = is_in_bounds(p0, bounds);
  const bool p1_in = is_in_bounds(p1, bounds);
  if (!p0_in && !p1_in) {
    // both points are outside bounds, do not draw
    return;
  }
  if (p0_in ^ p1_in) {
    p0 = {std::clamp(p0.x, bounds.horizontal.min, bounds.horizontal.max),
          std::clamp(p0.y, bounds.vertical.min, bounds.vertical.max)};
    p1 = {std::clamp(p1.x, bounds.horizontal.min, bounds.horizontal.max),
          std::clamp(p1.y, bounds.vertical.min, bounds.vertical.max)};
  }
  display::draw_line(p0.x, p0.y, p1.x, p1.y, color);
  // display::set_pixel(p1.x, p1.y, color);
  // display::set_pixel(p2.x, p2.y, color);
}

void draw_mark(GpsSettings *&plot_settings, MapPoint &point,
               const MapBounds &bounds) {
  if (point.x - 1 < bounds.horizontal.min ||
      point.x + 1 > bounds.horizontal.max ||
      point.y - 1 < bounds.vertical.min || point.y + 1 > bounds.vertical.max) {
    if (!is_in_bounds(point, bounds)) {
      return;
    }
    display::set_pixel(point.x, point.y, plot_settings->color);
  } else {
    display::set_pixel(point.x, point.y, plot_settings->color);
    display::set_pixel(point.x - 1, point.y, plot_settings->color);
    display::set_pixel(point.x + 1, point.y, plot_settings->color);
    display::set_pixel(point.x, point.y - 1, plot_settings->color);
    display::set_pixel(point.x, point.y + 1, plot_settings->color);
  }
}

MapPoint convertToMap(const Gps_Graph::Point &gps_point,
                      const Bounds<float> &gps_bounds,
                      const Bounds<mapType> &map_bounds) {
  MapPoint map_point;
  map_point.x = convert(gps_point.lon, gps_bounds.horizontal.min,
                        gps_bounds.horizontal.max, map_bounds.horizontal.min,
                        map_bounds.horizontal.max);
  map_point.y =
      map_bounds.vertical.max -
      convert(gps_point.lat, gps_bounds.vertical.min, gps_bounds.vertical.max,
              map_bounds.vertical.min, map_bounds.vertical.max);
  return map_point;
}

void render_gps_path(const void *settings) {
  GpsSettings *plot_settings = (GpsSettings *)settings;
  RingArray<Gps_Graph::Point> &gps_data = *((RingArray<Gps_Graph::Point> *)plot_settings->data);

  Gps_Graph::Point &pos = *((Gps_Graph::Point *)plot_settings->pos);


  if (gps_data.empty()) {
    TRACE_ABNORMAL(TRACE_VIEWS, "GPS Plot: No points to render\n");
    return;
  }

  TRACE_DEBUG(8, TRACE_VIEWS, "GPS Plot: Rendering %zu points\n",
              gps_data.size());

  const Bounds<float> gps_bounds = {{pos.lon - plot_settings->radius,
                                     pos.lon + plot_settings->radius},
                                    {pos.lat - plot_settings->radius,
                                     pos.lat + plot_settings->radius}};

  TRACE_DEBUG(8, TRACE_VIEWS,
              "GPS Plot: View bounds: lat[%.6f, %.6f], lon[%.6f, %.6f]\n",
              gps_bounds.vertical.min, gps_bounds.vertical.max,
              gps_bounds.horizontal.min, gps_bounds.horizontal.max);

  // Get frame dimensions
  const auto &frame = plot_settings->frame;

  const unsigned boundary =
      5; // pixels from edge to avoid drawing on the border

  const MapBounds map_bounds = {
      {frame.x + boundary, frame.x + (frame.width - boundary) - 1},
      {frame.y + boundary, frame.y + (frame.height - boundary) - 1}};

  MapPoint previous_point = {0, 0};
  MapPoint current_point = {0, 0};
  int points_outside = 0;

  previous_point = convertToMap(gps_data[0], gps_bounds, map_bounds);
  for (size_t i = 1; i < gps_data.size(); i++) {
    auto point = gps_data[i];


    current_point = convertToMap(point, gps_bounds, map_bounds);
    if(gps_data[i-1].lat == 0 && gps_data[i-1].lon == 0) {
      previous_point = current_point;
      continue;
    }

    draw_line_between_points(previous_point, current_point,
                             plot_settings->color2, map_bounds);
    draw_mark(plot_settings, current_point, map_bounds);

    previous_point = current_point;
  }

  auto posMap = convertToMap(pos, gps_bounds, map_bounds);
  draw_mark(plot_settings, posMap, map_bounds);


  TRACE_DEBUG(8, TRACE_VIEWS,
              "GPS Plot: %d points outside radius (%.2f%% of total)\n",
              points_outside,
              (float)points_outside * 100.0f / (float)gps_data.size());
}