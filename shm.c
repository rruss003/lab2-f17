#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

// int shm_open(int id, char **pointer) {

// // you write this
// // Look through the shm_table to see if the id we are opening already exists
// int i = 0;
// int index = -1;
// acquire(&(shm_table.lock));
// for (i=0; i<64; i++){
//   if(shm_table.shm_pages[i].id == id){
//     index = i;
//     break;
//   }
// }
// struct proc* p = myproc();
// if(index > -1){
//   // Case 1
//   mappages(p->pgdir, (void*)PGROUNDUP(p->sz), PGSIZE, V2P(shm_table.shm_pages[index].frame), PTE_W|PTE_U);
//   shm_table.shm_pages[index].refcnt++;
//   *pointer=(char *)PGROUNDUP(p->sz);
// //   p->sz += PGSIZE;
//   cprintf("test %d\n", index);
// }
// else{
//   // Case 2
//   for(i = 0; i<64; i++){
//     if(shm_table.shm_pages[i].id == 0){
//       shm_table.shm_pages[i].id = id;
//       shm_table.shm_pages[i].frame = kalloc();
//       shm_table.shm_pages[i].refcnt = 1;
//       memset(shm_table.shm_pages[i].frame, 0, PGSIZE);
//       mappages(p->pgdir, (void*)(p->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
//       *pointer=(char *)PGROUNDUP(p->sz);
//       p->sz += PGSIZE;
//       break;
//     }
//   }
// }
  
// release(&(shm_table.lock));
// return 0; //added to remove compiler warning -- you should decide what to return
// }


// int shm_close(int id) {
// int i=0;
// initlock(&(shm_table.lock), "SHM lock");
// acquire(&(shm_table.lock));
// for (i=0; i<64; i++){
//   if(shm_table.shm_pages[i].id == id){
//     shm_table.shm_pages[i].refcnt--;
//     if(shm_table.shm_pages[i].refcnt == 0){
//       shm_table.shm_pages[i].id =0;
//       shm_table.shm_pages[i].frame =0;
//     }
//     break;
//   }
// }
// release(&(shm_table.lock));

// return 0; //added to remove compiler warning -- you should decide what to return
// }
int shm_open(int id, char **pointer) {

//you write this
	acquire(&(shm_table.lock));
//case 1
	int i;
	for(i = 0; i < 64; i++){
		if(shm_table.shm_pages[i].id == id){
			//adds mapping between the virtual address and the physical address of the page in the table, and map it to an available page on the virtual address space.
			mappages(myproc()->pgdir, (void*)PGROUNDUP(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
			shm_table.shm_pages[i].refcnt++;
			*pointer = (char*) PGROUNDUP(myproc()->sz);
			myproc()->sz += PGSIZE;
			release(&(shm_table.lock));
			return 0;
		}
	}
//case2
	int j;
	for(j = 0; j < 64; j++){
		if(shm_table.shm_pages[j].id == 0){
			shm_table.shm_pages[j].id = id;
			shm_table.shm_pages[j].frame = kalloc();
			shm_table.shm_pages[j].refcnt = 1;
			memset(shm_table.shm_pages[j].frame, 0, PGSIZE);
			mappages(myproc()->pgdir, (void*)PGROUNDUP(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[j].frame), PTE_W|PTE_U);
			*pointer = (char*)PGROUNDUP(myproc()->sz);
			myproc()->sz += PGSIZE;
			release(&(shm_table.lock));
			return 0;

		}	
	}




//return 0; //added to remove compiler warning -- you should decide what to return
	return -1; //for error
}


int shm_close(int id) {
//you write this too!
	initlock(&(shm_table.lock), "SHM lock");
	acquire(&(shm_table.lock));
	int i;
	for(i = 0; i < 64; i++){
		if(shm_table.shm_pages[i].id == id){
			shm_table.shm_pages[i].refcnt--;
		
			if(shm_table.shm_pages[i].refcnt > 0){
				release(&(shm_table.lock));
				return 0;
			}
		
			shm_table.shm_pages[i].frame = 0;
			shm_table.shm_pages[i].refcnt = 0;
			shm_table.shm_pages[i].id = 0;
	
			release(&(shm_table.lock));
			return 0;
		}

	}

	release(&(shm_table.lock));
	return 1;

//return 0; //added to remove compiler warning -- you should decide what to return
}

