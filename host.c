#include <stdio.h>
#include <stdlib.h>
#include <e-hal.h>
#include <stddef.h>
#include "synchronize.h"

int main(int argc, char* argv[])
{
	int r = 4, c = 4;
	e_mem_t emem;
	e_epiphany_t epi;

	e_init(NULL);
	e_alloc(&emem, 0x00000000, 0x00400000);
	printf("address=0x%x Offsets - Physical host address=%x epiphany address=%x\n", &emem, emem.phy_base, emem.ephy_base);

	software_interrupt_t intr_vector;
	intr_vector.start = 0;
	intr_vector.done = 0;
	intr_vector.size = sizeof(software_interrupt_t);
	intr_vector.counter = 0;
	intr_vector.row_id = r;
	intr_vector.col_id = c;
	intr_vector.finish = 0;
	e_write(&emem, 0, 0, 0x00000000, &intr_vector, sizeof(intr_vector));
	
	// Start the program
	e_open(&epi, 0, 0, r, c);
	e_reset_system();
	e_load_group("sync_epi.elf", &epi, 0, 0, r, c, E_TRUE);
	int buf[16] = {0};
	int is_exec_complete = 0;


	// Wait until the program is finished, then display final counter value
	while(1)
	{
		e_read(&emem, 0, 0, offsetof(software_interrupt_t, done), &intr_vector.done, sizeof(int));
		do
		{
			e_read(&emem, 0, 0, offsetof(software_interrupt_t, finish), &intr_vector.finish, sizeof(int));
			if (intr_vector.done == 1){
				is_exec_complete = 1;
				break;			
			}
		} while (intr_vector.finish == 0);
		if (is_exec_complete == 1) break;
		e_read(&emem, 0, 0, offsetof(software_interrupt_t, row_id), &intr_vector.row_id, sizeof(int));
		e_read(&emem, 0, 0, offsetof(software_interrupt_t, col_id), &intr_vector.col_id, sizeof(int));
		e_read(&emem, 0, 0, offsetof(software_interrupt_t, counter), &intr_vector.counter, sizeof(int));
		printf("Executed core=[%d %d] counter value: %d\n", intr_vector.row_id, intr_vector.col_id, intr_vector.counter);
		intr_vector.finish = 0;
		e_write(&emem, 0, 0, offsetof(software_interrupt_t, finish), &intr_vector.finish, sizeof(int));
		e_read(&emem, 0, 0, sizeof(intr_vector), &buf[0], sizeof(int) * 16);
		for(int i = 0; i < 16; i++){
			printf("Core %d is %d\n", i, buf[i]);
		}
	}
	e_read(&emem, 0, 0, offsetof(software_interrupt_t, counter), &intr_vector.counter, sizeof(int));
	printf("Final counter value: %d\n", intr_vector.counter);

	e_close(&epi);
	e_free(&emem);
	e_finalize();
	return 0;
}
