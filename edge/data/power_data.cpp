#include "power_data.h"

PowerData::PowerData(time_t timestamp, double avg)
    : BaseData(timestamp, avg, "kWh")
{
}

double PowerData::getValue()
{
  return avg;
}