#pragma once
void storage_reset(void);
void storage_fail_next_write(int bytes);
void storage_corrupt(unsigned key);
