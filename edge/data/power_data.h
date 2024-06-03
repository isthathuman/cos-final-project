#ifndef __POWER_DATA_H__
#define __POWER_DATA_H__

#include "base_data.h"

class PowerData : public BaseData
{
public:
  PowerData(time_t timestamp, double avg);
  double getValue();
};

#endif /* __POWER_DATA_H__ */