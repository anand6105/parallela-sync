#include <e_lib.h>
#include "synchronize.h"

e_mutex_t m SECTION(".data_bank1");

int main(void)
{
	unsigned int r, c;
	e_coords_from_coreid(e_get_coreid(), &r, &c);
	unsigned int e_id = r * 4 + c;
	software_interrupt_t* sw_intr = (software_interrupt_t*)SHARED_DRAM;
	int* done = (int*)DRAM_OFFSET(sw_intr->size);

	// initialize mutex in core 0
	if (e_id == 0)
	{
		int i;
		for (i = 0; i < 16; i++)
			done[i] = 0;

		sw_intr->mutex = e_get_global_address(r, c, &m);
		e_mutex_init(0, 0, sw_intr->mutex, NULL);

		sw_intr->start = 1;
	}
	else
		while (sw_intr->start == 0);

	// Increment the counter.  Adding some delay using for loop
	e_mutex_lock(0, 0, sw_intr->mutex);
	int local = sw_intr->counter;
	volatile int i;
	for (i = 0; i < 500000; i++);
	sw_intr->counter = local + (r + c);
	sw_intr->row_id = r;
	sw_intr->col_id = c;
	sw_intr->finish = 1;
	done[e_id] = 1;
	while(sw_intr->finish == 1);
	e_mutex_unlock(0, 0, sw_intr->mutex);


	// Wait for other cores to complete
	if (e_id == 0)
	{
		int sum;
		do
		{
			sum = 0;
			int i;
			for (i = 0; i < 16; i++)
				sum += done[i];
			done[16] = sum;
		} while (sum < 16);
		sw_intr->done = 1;
	}

	return 0;
}

