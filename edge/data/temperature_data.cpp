#include "temperature_data.h"

TemperatureData::TemperatureData(time_t timestamp, double min, double max, double avg)
    : BaseData(timestamp, avg, "celcius")
{
  this->min = min;
  this->max = max;
  this->avg = avg;
}
