#include "edge.h"
#include "opcode.h"
#include <iostream>
#include <ctime>
using namespace std;

Edge::Edge()
{
  this->dr = new DataReceiver();
  this->nm = new NetworkManager();
  this->pm = new ProcessManager();
}

Edge::~Edge()
{
  delete this->dr;
  delete this->nm;
  delete this->pm;
}

Edge::Edge(const char *addr, int port, int mode, time_t timestart, time_t endtime)
{
  this->dr = new DataReceiver();
  this->nm = new NetworkManager(addr, port);
  this->pm = new ProcessManager();
  this->mode = mode;
  this->timestart = timestart;
  this->endtime = endtime;
}

void Edge::init()
{
  this->dr->init();
  this->nm->init();
  this->pm->init();
}

void Edge::run()
{
  time_t curr, before_end;
  uint8_t opcode;
  uint8_t *data;
  DataSet *ds;
  int dlen;
  opcode = OPCODE_DONE;

  cout << "[*] Running the edge device:" << this->mode << endl;
  if (this->mode != 3)
  {
    cout << "when to start" << this->timestart << endl;
    cout << "when to end" << this->endtime << endl;
    cout << "mode: " << this->mode << endl;
    curr = this->timestart;
  }
  int cnt = 0;
  if (this->mode == 1)
  {
    while (opcode != OPCODE_QUIT)
    {
      before_end = this->endtime;
      ds = this->dr->getDataSet(curr);
      data = this->pm->processData(ds, &dlen);
      cout << data << endl;
      this->endtime = this->nm->sendData(data, dlen, 15, this->mode, this->endtime, curr);

      cout << "haha";
      opcode = this->nm->receiveCommand();
      cout << cnt++ << endl;
      cout << before_end << " " << this->endtime << " " << curr << endl
           << endl;
      curr += 86400;
      if (before_end != 0 && this->endtime == 0)
      {
        curr -= 86400;
      }
    }
  }
  else if (this->mode == 2)
  {
    while (opcode != OPCODE_QUIT)
    {
      before_end = this->endtime;
      ds = this->dr->getDataSet(curr);
      data = this->pm->processData(ds, &dlen);
      this->endtime = this->nm->sendData(data, dlen, 15, this->mode, this->endtime, curr);
      opcode = this->nm->receiveCommand();
      cout << cnt++ << endl;
      cout << before_end << " " << this->endtime << " " << curr << endl
           << endl;

      curr += 86400;
      if (before_end != 0 && this->endtime == 0)
      {
        curr -= 86400;
      }
    }
  }
  else if (this->mode == 3)
  {
    ds = this->dr->getDataSet(curr);
    data = this->pm->processData(ds, &dlen);
    endtime = this->nm->sendData(data, dlen, 15, this->mode, this->endtime, curr);
    opcode = this->nm->receiveCommand();
    cout << opcode << endl;
    cout << cnt++ << endl;
  }
  else
  {
    cout << "Wrong Mode" << endl;
    return;
  }

  cout << "[*] End running" << endl;
}
