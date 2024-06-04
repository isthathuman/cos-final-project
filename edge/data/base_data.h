#ifndef __BASE_DATA_H__
#define __BASE_DATA_H__

#include <ctime>
#include <string>

class BaseData
{
protected:
    time_t timestamp;
    double avg;
    std::string unit;
    BaseData *next;
    double min;
    double max;

public:
    BaseData(time_t timestamp, double avg, std::string unit);
    virtual ~BaseData();

    void setNext(BaseData *data);
    BaseData *getNext();

    void setValue(double value);
    void setTimestamp(time_t timestamp);
    time_t getTimestamp();

    std::string getUnit();
    void setMin(double min);

    double getMin();

    void setMax(double max);

    double getMax();

    double getAvg();
    // virtual double getValue() = 0;
};

#endif /* __BASE_DATA_H__ */