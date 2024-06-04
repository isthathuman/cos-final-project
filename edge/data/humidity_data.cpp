#include "humidity_data.h"

HumidityData::HumidityData(time_t timestamp, double min, double max, double avg)
    : BaseData(timestamp, avg, "%")
{
  this->min = min;
  this->max = max;
  this->avg = avg;
}