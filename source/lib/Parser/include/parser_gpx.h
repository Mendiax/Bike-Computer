#pragma once

#include <string>
#include <vector>

/**
 * @brief Represents a point in a GPX track with coordinates and elevation
 */
struct GpxPoint {
    double lat;         ///< Latitude in degrees
    double lon;         ///< Longitude in degrees
    double elevation;   ///< Elevation in meters

    GpxPoint() : lat(0), lon(0), elevation(0) {}

    /**
     * @brief Construct a track point
     * @param latitude Latitude in degrees
     * @param longitude Longitude in degrees
     * @param elevation Elevation in meters
     */
    GpxPoint(double latitude, double longitude, double elevation)
        : lat(latitude), lon(longitude), elevation(elevation) {}
};

/**
 * @brief Contains a GPX track with points and optional name
 */
struct GpxTrack {
    std::string name;                ///< Name of the track
    std::vector<GpxPoint> points;    ///< Vector of track points

    GpxTrack() = default;
    ~GpxTrack() = default;
};

/**
 * @brief Initialize the GPX parser state
 */
void init_gpx_parser();

/**
 * @brief Parse a chunk of GPX data
 *
 * @param chunk The GPX data chunk to parse
 * @param size Size of the chunk in bytes
 * @return int 0 on success, non-zero on error
 */
int parse_gpx_chunk(const char* chunk, size_t size);

/**
 * @brief Get the parsed track object
 *
 * The caller takes ownership of the returned pointer
 *
 * @return GpxTrack* Pointer to the track or nullptr if parsing failed
 */
GpxTrack* get_parsed_track();

/**
 * @brief Reset the parser state
 */
void reset_gpx_parser();
