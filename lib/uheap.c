
#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
uint32 nph = (USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE;
uint32 hp[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE] = {0};
uint32 hm = 0;
uint32 All_User_Addresses_Adds[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];
uint32 All_User_Addresses_Size[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];
int Counter = 0;
uint32 FIRSTFIT(uint32 size_pages)
{
	uint32 end,start,empty = 0,found = 0;
	for (uint32 i = 0; i < nph; i++)
	{
		if (hp[i] == 0)
		{
			empty++;
		}
		else
		{
			empty = 0;
		}

		if (empty == size_pages)
		{
			end=i;
			start=(end-size_pages)+1;
			found = 1;
			break;
		}
	}
	if (found == 1)
	{
		for ( int i = start  ; i <= end; i++)
		{
			hp[i] = 1;
		}

		start *= PAGE_SIZE ;
		start += USER_HEAP_START ;
		return start ;
	}
	else
	{
		return -1;
	}
}





void* malloc(uint32 size)
{
	//TODO: [PROJECT 2017 - [5] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 sizenum = size / PAGE_SIZE;
	uint32 First_Free_VA_In_UHeap = FIRSTFIT(sizenum);
	if(First_Free_VA_In_UHeap != -1)
	{
		All_User_Addresses_Adds[Counter] = First_Free_VA_In_UHeap;
		All_User_Addresses_Size[Counter] = size;
		Counter++;
		uint32 start=First_Free_VA_In_UHeap;
		sys_allocateMem(First_Free_VA_In_UHeap,size);
		return (void*)start;
	}
	return NULL;
	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer

}


void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Creation] smalloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 sizenum = size / PAGE_SIZE;
	uint32 First_Free_VA_In_UHeap = FIRSTFIT(sizenum);
	if(First_Free_VA_In_UHeap != -1)
	{
		All_User_Addresses_Adds[Counter] = First_Free_VA_In_UHeap;
		All_User_Addresses_Size[Counter] = size;
		Counter++;
		uint32 start=First_Free_VA_In_UHeap;
		int test=sys_createSharedObject(sharedVarName,size,isWritable,(void*)First_Free_VA_In_UHeap);
		if(test>=0)
		{
			return (void*)start;
		}
	}
	return NULL;
	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_createSharedObject(...) to invoke the Kernel for allocation of shared variable
	//		sys_createSharedObject(): if succeed, it returns the ID of the created variable. Else, it returns -ve
	//	4) If the Kernel successfully creates the shared variable, return its virtual address
	//	   Else, return NULL

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT 2017 - [6] Shared Variables: Get] sget() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");
	int ret;
	int size;
	size = sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
	if(size >= 0)
	{
		size = ROUNDUP(size, PAGE_SIZE);
		uint32 sizenum = size / PAGE_SIZE;
		uint32 First_Free_VA_In_UHeap = FIRSTFIT(sizenum);
		if(First_Free_VA_In_UHeap != -1)
		{
			All_User_Addresses_Adds[Counter] = First_Free_VA_In_UHeap;
			All_User_Addresses_Size[Counter] = size;
			Counter++;
			uint32 Return_Address = First_Free_VA_In_UHeap;
			ret = sys_getSharedObject(ownerEnvID,sharedVarName,(void*)Return_Address);
			if(ret >= 0)
			{
				for(int i = 0;i < size;i += PAGE_SIZE)
				{
					First_Free_VA_In_UHeap += PAGE_SIZE;
				}
				return (void*)Return_Address;
			}
		}
	}
	return NULL;
	// Steps:
	//	1) Get the size of the shared variable (use sys_getSizeOfSharedObject())
	//	2) If not exists, return NULL
	//	3) Implement FIRST FIT strategy to search the heap for suitable space
	//		to share the variable (should be on 4 KB BOUNDARY)
	//	4) if no suitable space found, return NULL
	//	 Else,
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//

	//This function should find the space for sharing the variable
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	//change this "return" according to your answer

}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [5] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 start;
	int size;
	for(uint32 i=0; i<Counter; i++)
	{
		if((void*)All_User_Addresses_Adds[i] == virtual_address)
		{
			sys_freeMem(All_User_Addresses_Adds[i] ,All_User_Addresses_Size[i]);
			start = All_User_Addresses_Adds[i];
			size = All_User_Addresses_Size[i] / PAGE_SIZE;
			start -= USER_HEAP_START;
			start = start / PAGE_SIZE;
			for ( int i = start  ; i <= (start + size) - 1; i++)
			{
				hp[i] = 0;
			}
			All_User_Addresses_Adds[i] = (uint32)NULL;
			All_User_Addresses_Size[i] = -1;
			break;
		}
	}
	//you should get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=============
// [1] sfree():
//=============
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - BONUS4] Free Shared Variable [User Side]
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");

	//	1) you should find the ID of the shared variable at the given address
	//	2) you need to call sys_freeSharedObject()

}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

}
