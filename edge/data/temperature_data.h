#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "base_data.h"

class TemperatureData : public BaseData
{
private:
  double min;
  double max;

public:
  TemperatureData(time_t timestamp, double min, double max, double avg);

  void setMin(double min);
  double getMin();

  void setMax(double max);
  double getMax();
  double getAvg();
};

#endif /* __TEMPERATURE_H__ */