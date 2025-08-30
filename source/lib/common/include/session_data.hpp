#pragma once

#include "session.hpp"


struct Packet{
    Sensor_Data sensors;
    Session session;
};

struct Track{
    std::string name;
    // TODO
    Gps_Graph track_graph;
};

struct SessionData
{
    Sensor_Data sensors;
    Session session;
    Track track;
};