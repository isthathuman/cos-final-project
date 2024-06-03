#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>
using namespace std;

ProcessManager::ProcessManager()
{
  this->num = 0;
}

void ProcessManager::init()
{
}

// TODO: You should implement this function if you want to change the result of the aggregation
uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
{
  uint8_t *ret;
  int num;
  HouseData *house;
  TemperatureData *tdata;
  HumidityData *hdata;
  PowerData *pdata;
  char buf[BUFLEN];
  int min_humid, min_temp, max_temp, max_power, max_humid, month, avg_temp;
  time_t ts;
  struct tm *tm;

  tdata = ds->getTemperatureData();
  hdata = ds->getHumidityData();
  num = ds->getNumHouseData();

  min_temp = (int)tdata->getMin();
  max_temp = (int)tdata->getMax();

  avg_temp = (int)tdata->getAvg();

  min_humid = (int)hdata->getMin();
  max_humid = (int)hdata->getMax();

  max_power = 0;
  for (int i = 0; i < num; i++)
  {
    house = ds->getHouseData(i);
    pdata = house->getPowerData();
    int tmp = (int)pdata->getValue();

    if (tmp > max_power)
      max_power = tmp;
  }

  ts = ds->getTime();
  tm = localtime(&ts);
  month = tm->tm_mon + 1;

  // JSON 형식으로 데이터 생성
  snprintf(buf, BUFLEN, "{\"min_temp\":%d,\"min_humid\":%d,\"max_temp\":%d,\"max_humid\":%d,\"power\":%d,\"timestamp\":%ld,\"month\":%d}",
           min_temp, min_humid, max_temp, max_humid, max_power, ts, month);

  // 비트로 만든거 추가

  *dlen = strlen(buf);
  ret = (uint8_t *)malloc(*dlen);
  memcpy(ret, buf, *dlen);

  return ret;
}

// uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
// {
//   uint8_t *ret;
//   int num;
//   HouseData *house;
//   TemperatureData *tdata;
//   HumidityData *hdata;
//   PowerData *pdata;
//   char buf[BUFLEN];
//   int min_temp, max_temp, max_power, min_humid, max_humid, month;
//   double total_temp = 0.0, total_humid = 0.0, average_temp, average_humid;
//   time_t ts;
//   struct tm *tm;

//   tdata = ds->getTemperatureData();
//   hdata = ds->getHumidityData();
//   num = ds->getNumHouseData();

//   min_temp = (int)tdata->getMin();
//   max_temp = (int)tdata->getMax();
//   min_humid = (int)hdata->getMin();
//   max_humid = (int)hdata->getMax();
//   average_temp = (int)tdata->getAvg();
//   average_humid = (int)hdata->getAvg();

//   max_power = 0;
//   for (int i = 0; i < num; i++)
//   {
//     house = ds->getHouseData(i);
//     pdata = house->getPowerData();
//     int tmp = (int)pdata->getValue();

//     if (tmp > max_power)
//       max_power = tmp;
//   }

//   ts = ds->getTime();
//   tm = localtime(&ts);
//   month = tm->tm_mon + 1;

//   // JSON 형식으로 데이터 생성
//   snprintf(buf, BUFLEN, "{\"average_temp\":%.2f,\"average_humid\":%.2f,\"power\":%d,\"timestamp\":%ld,\"month\":%d}",
//            average_temp, average_humid, max_power, ts, month);

//   // 비트로 만든거 추가

//   *dlen = strlen(buf);
//   ret = (uint8_t *)malloc(*dlen);
//   memcpy(ret, buf, *dlen);

//   return ret;
// }