#ifndef __EDGE_H__
#define __EDGE_H__

#include "setting.h"
#include "data_receiver.h"
#include "network_manager.h"
#include "process_manager.h"

class Edge
{
private:
  DataReceiver *dr;
  NetworkManager *nm;
  ProcessManager *pm;
  int mode;
  time_t timestart, endtime;

public:
  Edge();
  Edge(const char *addr, int port, int mode, time_t timestart, time_t endtime);

  ~Edge();

  void setAddress(const char *addr);
  const char *getAddress();

  void setPort(int port);
  int getPort();

  void init();
  void run();
};

#endif /* __EDGE_H__ */
