#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "base_data.h"

class TemperatureData : public BaseData
{
private:
public:
  TemperatureData(time_t timestamp, double min, double max, double avg);
};

#endif /* __TEMPERATURE_H__ */