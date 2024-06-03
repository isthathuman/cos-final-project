#include "temperature_data.h"

TemperatureData::TemperatureData(time_t timestamp, double min, double max, double avg)
    : BaseData(timestamp, avg, "celcius")
{
  this->min = min;
  this->max = max;
}

void TemperatureData::setMin(double min)
{
  this->min = min;
}

double TemperatureData::getMin()
{
  return this->min;
}

void TemperatureData::setMax(double max)
{
  this->max = max;
}

double TemperatureData::getMax()
{
  return this->max;
}

double TemperatureData::getAvg()
{
  return this->avg;
}