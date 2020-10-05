#pragma once
#include <vector>
#include <map>

using namespace std;

#define MEMORY_SIZE 4096
#define PAGE_SIZE 512
#define CLASS_MIN 4


class Allocator
{
private:

	enum class PageState {
		Free,
		Divided_Into_Blocks,
		Filled_multipage_block
	};

	struct PageDescriber {
		PageState pageState;
		int blockClass;
		int* firstFreeBlock;
		int freeBlocks;
	};

	char memory[MEMORY_SIZE];

	vector <void*> FreePages;
	map <void*, PageDescriber> PagesDescribers;
	map <int, vector <void*>> classDescFreeBlock;

	void define_free_pages();
	void define_describers();
	void define_class_describers();
	void change_FreePages();
	int count_class_block(size_t size);
	void divided_page_into_blocks(int blockClass, void* page);
	void* get_free_block(void* page);
	void* multipages_block(size_t size);
	void* flag_multipages(void* start, int pages);
	void* fing_page_with_block(void* addr);
	void free_small_block(void* page, void* addr);
	void free_multiblock(void* page);
	void mem_copy(void* from, void* to);
	void mem_move(void* from, void* to, int length);
	void update_freePages();
	
public:
	Allocator();

	void* mem_alloc(size_t size);
	void mem_free(void* addr);
	void* mem_realloc(void* addr, size_t size);
	void mem_show();
};
