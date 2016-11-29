#include <system.h>
#include <kernlib.h>

extern void write_cr3(unsigned long*);
extern void write_cr0(unsigned long);
extern unsigned long read_cr0();

typedef struct pageInfo pageInfo;
typedef struct mallocInfo mallocInfo; 

struct pageInfo
{
	u_long* page_start;
};

struct mallocInfo
{
	u_long* location;
	uint32_t size;
	short flags;
};

// Paging directory
unsigned long *page_directory = (unsigned long *) LOC_PAGE_DIR;

// Memory administration vars
pageInfo* pageInfoStart = 0;
pageInfo* pageInfoEnd = 0;
pageInfo* pageInfoPnt = 0;
mallocInfo* malloInfocStart = 0;
mallocInfo* mallocInfoEnd = 0;
u_long* memPnt = 0;
int _currentPDEntry = 0;
int _currentAddress = 0;
unsigned long *pageTablePnt = (unsigned long *) LOC_PAGE_TABLE;

/* Saves malloc info into the Malloc Info list,
*  this way we can monitor where and how 
*  much memory has been allocated. */ 
void pushback_mallocInfo(u_long* address, uint32_t size, short flags)
{
	// If it's the first entry we need to set up the list.
	if (malloInfocStart == 0)
	{
		malloInfocStart =  (mallocInfo*) LOC_MALLOC_INFO;
		malloInfocStart->location = address;
		malloInfocStart->size = size;
		malloInfocStart->flags = flags;
		mallocInfoEnd = malloInfocStart;
	}
	else
	{
		// else we just add it to the end of the list
		mallocInfoEnd++;
		mallocInfoEnd->location = (u_long*)address;
		mallocInfoEnd->size = size;
		mallocInfoEnd->flags = flags;
	}
		
}

/* Pages are used to map memory into a specific address space.
* This function is called when new pages are mapped into the address space.*/
void pushback_pageInfo(u_long address)
{
	if (pageInfoStart == 0)
	{
		pageInfoStart =  (pageInfo*) LOC_PAGE_INFO;
		pageInfoStart->page_start = (u_long*)address;
		pageInfoPnt = pageInfoStart;
		pageInfoEnd = pageInfoStart;
	}
	else
	{
		pageInfoEnd++;
		pageInfoEnd->page_start = (u_long*)address;
	}
		
}

/* Maps n pages to kernel memory */
void map_mem(int pages)
{
	int i;
	unsigned long *pTableStart = pageTablePnt;
	for(i = 0; i<pages; i++)
	{
		pushback_pageInfo(_currentAddress);
		*pageTablePnt++ = _currentAddress | 3; // attribute set to: supervisor level, read/write, present(011 in binary)
		_currentAddress = _currentAddress + PAGE_SIZE; // 4096 = 4kb
	};
	page_directory[_currentPDEntry] = (int) pTableStart; // attribute set to: supervisor level, read/write, present(011 in binary)
	page_directory[_currentPDEntry] = page_directory[_currentPDEntry] | 3;
	_currentPDEntry++;
}

/* Removes a page from the free page information list. 
* If there are no more pages left we have a serious problem. */
void* pop_pageInfo()
{
	map_mem(1);
	if (pageInfoPnt > pageInfoEnd)
		panic("Out of memory!");
	u_long* ret = pageInfoPnt->page_start;
	pageInfoPnt++;
	return (void*)ret;
}

/* Setup paging, will reserve 4mb for the kernel. */
void install_paging()
{
	unsigned int i;

	// map kernel mem (first 4 mb)
	map_mem(1025); // 1 extra to give malloc a start
	pageInfoPnt = pageInfoEnd; // malloc can start giving out mem from there...
	
	for(i=2; i<1024; i++)
	{
		page_directory[i] = 0 | 2; // attribute set to: supervisor level, read/write, not present(010 in binary)
	};
	
	write_cr3(page_directory); // put that page directory address into CR3
	write_cr0(read_cr0() | 0x80000000); // set the paging bit in CR0 to 1
}

/* Requests more memory pages to be used, this will pop pages from the free page information list. */
void* more_core(size_t size)
{
	void* ret = pop_pageInfo();
	size -= PAGE_SIZE;
	if (size > 0)
	{
		while(size > 0)
		{
			pop_pageInfo();
			size -= PAGE_SIZE;
		}
	}
	return ret;
}

/* This function will free previously allocated memory. No checks are done if this is allowed! */
void free(void* pnt)
{
	mallocInfo* mPnt = malloInfocStart;
	while (mPnt->size != 0)
	{
		if (mPnt->location == (u_long*)pnt)
		{
			mPnt->flags--;
			break;
		}
		mPnt++;
	}
}

/* Checks if there is a free memory spot in the existing list. 
* This will consume spots that are way too large for the requested size! */
void* malloc_check(uint32_t size)
{
	mallocInfo* mPnt = malloInfocStart;
	while (mPnt->size != 0)
	{
		if (mPnt->size >= size && mPnt->flags == 0x02)
		{
			mPnt->flags++;
			return mPnt->location;
		}
		mPnt++;
	}
	return 0;
}

/* Reserves memory and returns a pointer to the newly reserved memory. */
void* malloc(uint32_t size)
{
	if (memPnt == 0)
		memPnt = pageInfoPnt->page_start;
	
	u_long* ret = malloc_check(size);
	if (ret)
		return ret;
	
	if ((int)((unsigned long*)memPnt+size) > (int)(pageInfoPnt->page_start))
		more_core((int)((unsigned long*)memPnt)+size - (int)(pageInfoPnt->page_start));

	ret = memPnt;
	memPnt = (void*)ret+size;
	
	// save info
	pushback_mallocInfo(ret, size, 0x03);
	return ret;
}

/* Prints all reserved memory slots using kprintf(). */
void memdump()
{
	mallocInfo* mPnt = malloInfocStart;
	while (mPnt->size != 0)
	{
		if (mPnt->size != 0)
			kprintf("Address: %x Size: %d Flags: %x\n", mPnt->location, mPnt->size, mPnt->flags);
		mPnt++;
	}
}
