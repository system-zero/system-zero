struct sysinfo {
  unsigned long uptime;
  unsigned long loads[3];
  unsigned long totalram;
  unsigned long freeram;
  unsigned long sharedram;
  unsigned long bufferram;
  unsigned long totalswap;
  unsigned long freeswap;
  unsigned short procs, pad;
  unsigned long totalhigh;
  unsigned long freehigh;
  unsigned mem_unit;
  char __reserved[256];
};
