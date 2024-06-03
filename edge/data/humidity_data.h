#ifndef __HUMIDITY_DATA_H__
#define __HUMIDITY_DATA_H__

#include "base_data.h"

class HumidityData : public BaseData
{
private:
  double min;
  double max;

public:
  HumidityData(time_t timestamp, double min, double max, double avg);

  void setMin(double min);
  double getMin();

  void setMax(double max);
  double getMax();

  double getAvg();
};

#endif /* __HUMIDITY_DATA_H__ */