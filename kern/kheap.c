#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

//=================================================================================//
//============================REQUIRED FUNCTION==================================//
//=================================================================================//



unsigned int First_Free_VA_In_KHeap = KERNEL_HEAP_START;

unsigned int All_Addresses_Adds[4096];
unsigned int All_Addresses_Size[4096];
int AddCounter = 0;

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kmalloc()

	All_Addresses_Adds[AddCounter] = First_Free_VA_In_KHeap ;
	All_Addresses_Size[AddCounter] = size;
	AddCounter++;
	unsigned int Return_Address;
	int Available_Size = (( KERNEL_HEAP_MAX - First_Free_VA_In_KHeap ));
	size = ROUNDUP(size, PAGE_SIZE);
	if (size <= Available_Size)
	{
	   Return_Address = First_Free_VA_In_KHeap;
	   for (int i = 0; i < size; i+=PAGE_SIZE)
	   {
           struct Frame_Info *f = NULL;
           allocate_frame(&f);
		   map_frame(ptr_page_directory,f,(void*)First_Free_VA_In_KHeap,PERM_WRITEABLE);
		   First_Free_VA_In_KHeap+=PAGE_SIZE ;
	   }
	   return (void*)Return_Address;
	}
	return NULL;
	//First Fit
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kfree()

    for ( int j = 0 ; j < AddCounter ; j++)
    {
    	if( All_Addresses_Adds[j] == (unsigned int)virtual_address )
    	{
    		for ( int i = 0 ; i < All_Addresses_Size[j]  ; i+=PAGE_SIZE)
    			{
    				unmap_frame(ptr_page_directory,(void *)virtual_address);
    					virtual_address +=PAGE_SIZE;
    			}
            break ;
    	}
    }

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_virtual_address()

	uint32 MyFrame = physical_address >> 12  ;
	uint32 *ptr_page_table = NULL;


		for ( uint32 i = KERNEL_HEAP_START; i < First_Free_VA_In_KHeap ; i+=PAGE_SIZE)
		{
			  			get_page_table(ptr_page_directory, (void*)i,  &ptr_page_table) ;
			if (ptr_page_table != NULL)
			{
				uint32 table_index = PTX(i);
				int add = ptr_page_table[table_index];
			    uint32 Current_Frame = add >> 12 ;

			    if( Current_Frame == MyFrame )
			  	  {
			    	return i;
			  	  }

			}
		}

	return 0;
}
unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_physical_address()

	uint32 *ptr_page_table = NULL;
	get_page_table(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	if (ptr_page_table != NULL)
	{
		uint32 table_index = PTX(virtual_address);
		uint32 fn = ptr_page_table[table_index] >> 12;
		uint32 pa = fn * PAGE_SIZE;
		uint32 offset = virtual_address << 20;
		offset = offset >> 20;
		int p = ptr_page_table[table_index] & PERM_PRESENT;
		return (pa + offset);
	}
	//change this "return" according to your answer
	return 0;
}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");

}
