#ifndef __HUMIDITY_DATA_H__
#define __HUMIDITY_DATA_H__

#include "base_data.h"

class HumidityData : public BaseData
{
private:
public:
  HumidityData(time_t timestamp, double min, double max, double avg);
};

#endif /* __HUMIDITY_DATA_H__ */