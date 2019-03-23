/*----------------------------------------------------------
    BOARD SELECTION
----------------------------------------------------------*/
#define STM8S_DISCOVERY 1
#define SDUINO_UNO      2
#define BOARD           STM8S_DISCOVERY


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#if BOARD == STM8S_DISCOVERY
  #warning STM8S-Discovery
  #include "STM8S105C6.h"
#elif BOARD == SDUINO_UNO
  #warning sduino-UNO
  #include "STM8S105K6.h"
#else
  #error please select supported device or adapt pinning
  #include <stophere>
#endif
