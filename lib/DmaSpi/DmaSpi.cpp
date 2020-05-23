#include "DmaSpi.h"

#if defined(KINETISK)
DmaSpi0 DMASPI0;
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
DmaSpi1 DMASPI1;
//DmaSpi2 DMASPI2;
#endif
#elif defined (KINETISL)
DmaSpi0 DMASPI0;
DmaSpi1 DMASPI1;
#elif defined(__IMXRT1062__)
DmaSpi0 DMASPI0;
DmaSpi1 DMASPI1;
DmaSpi2 DMASPI2;
#else 
#endif // defined