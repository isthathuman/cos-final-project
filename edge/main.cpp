#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <ctime>

#include "edge.h"
#include "data/data.h"
#include "setting.h"

void usage(uint8_t *pname)
{
  printf(">> Usage: %s [options]\n", pname);
  printf("Options\n");
  printf("  -a, --addr       Server's address\n");
  printf("  -p, --port       Server's port\n");
  printf("  -m, --mode      Mode for Dataset(1 = repeat not allowed, 2 = repeat allowed, 3 = result))\n");
  printf("  -s, --start      Start date (YYYYMMDD)\n");
  printf("  -e, --end      End date (YYYYMMDD)\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  int c, tmp, port, mode;
  uint8_t *pname, *addr;
  uint8_t eflag;
  time_t start_time = 1609459200, end_time = 0;
  Edge *edge;

  pname = (uint8_t *)argv[0];
  addr = NULL;
  port = -1;
  start_time = 0;
  eflag = 0;
  mode = 0;

  while (1)
  {
    int option_index = 0;
    static struct option long_options[] = {
        {"addr", required_argument, 0, 'a'},
        {"port", required_argument, 0, 'p'},
        {"mode", required_argument, 0, 'd'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {0, 0, 0, 0}};

    const char *opt = "a:p:d:se";

    c = getopt_long(argc, argv, opt, long_options, &option_index);

    if (c == -1)
      break;

    switch (c)
    {
    case 'a':
      tmp = strlen(optarg);
      addr = (uint8_t *)malloc(tmp);
      memcpy(addr, optarg, tmp);
      break;

    case 'p':
      port = atoi(optarg);
      break;

    case 'd':
      mode = atoi(optarg);
      break;

    case 's':
    {
      if (mode == 3)
        break;
      struct tm tm = {0};
      strptime(optarg, "%Y%m%d", &tm);
      start_time = mktime(&tm);
    }
    break;

    case 'e':
    {
      if (mode == 3)
        break;
      struct tm tm = {0};
      strptime(optarg, "%Y%m%d", &tm);
      end_time = mktime(&tm);
    }
    break;

    default:
      usage(pname);
    }
  }

  if (!addr)
  {
    printf("[*] Please specify the server's address to connect\n");
    eflag = 1;
  }

  if (port < 0)
  {
    printf("[*] Please specify the server's port to connect\n");
    eflag = 1;
  }

  if (mode != 3 && start_time == 0)
  {
    printf("[*] Please specify the start date (YYYYMMDD)\n");
    eflag = 1;
  }

  if (eflag)
  {
    usage(pname);
    exit(0);
  }

  edge = new Edge((const char *)addr, port, mode, start_time, end_time);
  edge->init();
  edge->run();

  return 0;
}