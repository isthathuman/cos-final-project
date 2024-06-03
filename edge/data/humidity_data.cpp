#include "humidity_data.h"

HumidityData::HumidityData(time_t timestamp, double min, double max, double avg)
    : BaseData(timestamp, avg, "%")
{
  this->min = min;
  this->max = max;
}

void HumidityData::setMin(double min)
{
  this->min = min;
}

double HumidityData::getMin()
{
  return this->min;
}

void HumidityData::setMax(double max)
{
  this->max = max;
}

double HumidityData::getMax()
{
  return this->max;
}

double HumidityData::getAvg()
{
  return this->avg;
}