#include "common_types.h"
#include "views/screenfunc/gps.h"
#include "display/driver.hpp"
#include "traces.h"
#include <algorithm>
#include <utility>
#include <cstdio>

void render_gps_path(const void *settings);

DrawFuncT get_draw_func_gps_path() {
    return render_gps_path;
}

template<typename T, typename O>
static inline O map(const T& val, const T& min, const T& max, const O& min_o, const O& max_o)
{
    //std::cout << "val_o: " << val_o << " " << val << " " << min << " " << max << " " << min_o << " " << max_o  << std::endl;
    // test input
    if (min == max)
    {
        return max_o;  // or max_o; since min == max, both represent the same point
    }
    if(val < min)
    {
        return min_o;
    }
    if(val > max)
    {
        return max_o;
    }
    O val_o = static_cast<O>((static_cast<double>(val - min) / static_cast<double>(max - min)) * static_cast<double>(max_o - min_o) + min_o);

    // test output
    if(val_o < min_o)
    {
        return min_o;
    }
    if(val_o > max_o)
    {
        return max_o;
    }
    return val_o; // not tested

}

void render_gps_path(const void *settings) {
    GpsSettings* plot_settings = (GpsSettings*)settings;
    Gps_Graph& gps_data = *((Gps_Graph*)plot_settings->data);

    if (gps_data.points.empty()) {
        TRACE_ABNORMAL(TRACE_VIEWS, "GPS Plot: No points to render\n");
        return;
    }

    TRACE_DEBUG(8, TRACE_VIEWS, "GPS Plot: Rendering %zu points\n", gps_data.points.size());

    // Find min/max coordinates to scale the points
    float min_lat = gps_data.pos.lat - plot_settings->radius;
    float max_lat = gps_data.pos.lat + plot_settings->radius;
    float min_lon = gps_data.pos.lon - plot_settings->radius;
    float max_lon = gps_data.pos.lon + plot_settings->radius;

    TRACE_DEBUG(8, TRACE_VIEWS, "GPS Plot: View bounds: lat[%.6f, %.6f], lon[%.6f, %.6f]\n",
                min_lat, max_lat, min_lon, max_lon);

    // Get frame dimensions
    const auto& frame = plot_settings->frame;

    int points_outside = 0;
    const unsigned boundary = 5; // pixels from edge to avoid drawing on the border
    // Draw lines between points
    int_fast16_t x0,y0,x1,y1;
    x0 = y0 = x1 = y1 = 0;

    int_fast16_t map_x_max = frame.x + (frame.width  - boundary) - 1;
    int_fast16_t map_x_min = frame.x + boundary;
    int_fast16_t map_y_max = frame.y + (frame.height - boundary) - 1;
    int_fast16_t map_y_min = frame.y + boundary;

    for (size_t i = 0; i < gps_data.points.size(); i++) {
        if (gps_data.points[i].lat < min_lat || gps_data.points[i].lat > max_lat ||
            gps_data.points[i].lon < min_lon || gps_data.points[i].lon > max_lon) {
            points_outside++;
            continue; // Skip points outside the defined radius
        }

        // Map GPS coordinates to screen coordinates for current and previous point
        x1 = map(gps_data.points[i].lon, min_lon, max_lon, map_x_min, map_x_max);
        y1 = map_y_max - map(gps_data.points[i].lat, min_lat, max_lat, map_y_min, map_y_max);
        if ((x1 < frame.x || x1 > frame.x + frame.width ||
             y1 < frame.y || y1 > frame.y + frame.height)) {
          // Both points are outside the drawable area; skip drawing
          TRACE_DEBUG(
              3, TRACE_VIEWS,
              "GPS Plot: Both points outside drawable area; skipping line\n");
            continue;
        }
        if(x0 != 0 || y0 != 0) {
            TRACE_DEBUG(8, TRACE_VIEWS, "GPS Plot: Drawing line from (%ld, %ld) to (%ld, %ld)\n",
                         (long)x0, (long)y0, (long)x1, (long)y1);
            display::draw_line(x0, y0, x1, y1, plot_settings->color2);
        }


        display::set_pixel(x1, y1, plot_settings->color);
        display::set_pixel(x1-1, y1, plot_settings->color);
        display::set_pixel(x1+1, y1, plot_settings->color);
        display::set_pixel(x1, y1-1, plot_settings->color);
        display::set_pixel(x1, y1+1, plot_settings->color);

        x0 = x1;
        y0 = y1;
    }

    TRACE_DEBUG(8, TRACE_VIEWS, "GPS Plot: %d points outside radius (%.2f%% of total)\n",
                points_outside,
                (float)points_outside * 100.0f / (float)gps_data.points.size());
}