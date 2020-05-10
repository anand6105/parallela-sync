#include <sys/types.h>

#ifdef __DEVICE__
extern const unsigned int _SHARED_DRAM_;
#define SHARED_DRAM ((unsigned int)&_SHARED_DRAM_)
#define DRAM_OFFSET(off) ((void*)(SHARED_DRAM + (off)))
#endif

typedef struct software_interrupt
{
	int start;
	int done;
	off_t size;
#ifdef __DEVICE__
	e_mutex_t* mutex;
#else
	void* mutex;
#endif
	int finish;
	int counter;
	int row_id;
	int col_id;
} software_interrupt_t;
