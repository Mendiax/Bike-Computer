#include "parser.hpp"

ForecastS::ForecastS(uint8_t days)
{
    this->data_len = days * DATA_PER_DAY;
    this->temperature_2m = nullptr;
    this->pressure_msl = nullptr;
    this->precipitation = nullptr;
    this->windspeed_10m = nullptr;
    this->winddirection_10m = nullptr;
    this->windgusts_10m = nullptr;
    this->dates = nullptr;
    this->sunrise = nullptr;
    this->sunset = nullptr;
    this->winddirection_10m_dominant = nullptr;

}
ForecastS::~ForecastS()
{
    #define DELETE_IF_NOT_0(x) if(x != 0) {delete[] x;}
    DELETE_IF_NOT_0(this->temperature_2m)
    DELETE_IF_NOT_0(this->pressure_msl)
    DELETE_IF_NOT_0(this->precipitation)
    DELETE_IF_NOT_0(this->windspeed_10m)
    DELETE_IF_NOT_0(this->winddirection_10m)
    DELETE_IF_NOT_0(this->windgusts_10m)
    DELETE_IF_NOT_0(this->dates)
    DELETE_IF_NOT_0(this->sunrise)
    DELETE_IF_NOT_0(this->sunset)
    DELETE_IF_NOT_0(this->winddirection_10m_dominant)

    #undef DELETE_IF_NOT_0
}