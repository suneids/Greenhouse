#ifndef FLASH_LL_F103C6T6
#define FLASH_LL_F103C6T6
#include "mcu_config.h"


int flash_ll_unlock();
int flash_ll_lock();
int flash_ll_erase_page(uint32_t page_addr);
int flash_ll_read_halfword(uint32_t addr, uint16_t *out);
int flash_ll_program_halfword(uint32_t addr, uint16_t data);

#endif
