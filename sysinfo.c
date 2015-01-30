#include <sys/resource.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

static const char mib_cputemp[]  = "dev.acpi_ibm.0.thermal";
static const char mib_battlife[] = "hw.acpi.battery.life";

static const wchar_t bargraph[] = L" ▁▂▃▄▅▆▇█";
static const int   bgmax = sizeof bargraph / sizeof *bargraph - 2;


void   usage(void);
double get_load(void);
int    get_cputemp(void);
int    get_battlife(void);
double get_diskusage(const char *diskname);


double
get_load(void)
{
  double load;
  getloadavg(&load, 1);
  return load;
}

int
get_cputemp(void)
{
  int temp;
  size_t templen = sizeof temp;
  sysctlbyname(mib_cputemp, &temp, &templen, NULL, 0);
  return temp;
}

int
get_battlife(void)
{
  int life;
  size_t lifelen = sizeof life;
  sysctlbyname(mib_battlife, &life, &lifelen, NULL, 0);
  return life;
}


int
main(void)
{
  setlocale(LC_ALL, "");

  time_t t = time(NULL);
  int blink;
  double load;
  int cputemp, battlife, blidx;
  const char *loadfmt, *cputempfmt, *battlifefmt, *battgraphfmt;

  blink = (int)t % 2;
  load = get_load();
  cputemp = get_cputemp();
  battlife = get_battlife();
  blidx = bgmax * (battlife / 100);

  loadfmt = "";
  if      (load > 5) loadfmt = blink?"fg=red,bold":"fg=black";
  else if (load > 3)  loadfmt = "fg=red";
  else if (load > 2)  loadfmt = "fg=yellow,bold";
  else if (load > 1)  loadfmt = "fg=green";
  else                loadfmt = "fg=blue,bold";

  cputempfmt = "";
  if      (cputemp > 90) cputempfmt = blink?"fg=red,bold":"fg=black";
  else if (cputemp > 80) cputempfmt = "fg=red";
  else if (cputemp > 70) cputempfmt = "fg=yellow,bold";
  else if (cputemp > 60) cputempfmt = "fg=green";
  else                   cputempfmt = "fg=blue,bold";

  battlifefmt = "";
  battgraphfmt = "";
  if      (blidx == 0)    battlifefmt = "fg=red",         battgraphfmt = blink?"fg=red,bg=black":"bg=red";
  else if (battlife < 33) battlifefmt = "fg=red",         battgraphfmt = "bg=red";
  else if (battlife < 66) battlifefmt = "fg=yellow,bold", battgraphfmt = "bg=yellow";
  else if (battlife < 90) battlifefmt = "fg=green",       battgraphfmt = "bg=green";
  else                    battlifefmt = "fg=blue,bold",   battgraphfmt = "bg=white,dim";

  wprintf(
    L"#[%s]⚒#[default]%.2f"
    " #[%s]♥#[default]%d°"
    " #[%s]⚡#[default]#[reverse,fg=black,bold,%s]%lc#[default]"
    , loadfmt, load
    , cputempfmt, cputemp
    , battlifefmt, battgraphfmt, bargraph[blidx]
  );
}

