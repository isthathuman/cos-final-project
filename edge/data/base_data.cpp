#include "base_data.h"

BaseData::BaseData(time_t timestamp, double avg, std::string unit)
{
    this->timestamp = timestamp;
    this->avg = avg;
    this->next = NULL;
    this->unit = unit;
}

BaseData::~BaseData() {}

void BaseData::setNext(BaseData *next)
{
    this->next = next;
}

BaseData *BaseData::getNext()
{
    return this->next;
}

void BaseData::setValue(double value)
{
    this->avg = value;
}

// double BaseData::getValue()
// {
//     return this->avg;
// }

void BaseData::setTimestamp(time_t timestamp)
{
    this->timestamp = timestamp;
}

time_t BaseData::getTimestamp()
{
    return this->timestamp;
}

std::string BaseData::getUnit()
{
    return this->unit;
}