#include "Allocator.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;


Allocator::Allocator() {
	define_free_pages();
	define_describers();
	define_class_describers();
};

void Allocator::define_free_pages() {

	int* p = (int*)memory;
	for (int i = 0; i < MEMORY_SIZE / PAGE_SIZE; i++) {

		FreePages.push_back(p);
		p += (int)PAGE_SIZE/4;
	}
};

void Allocator::define_describers() {
	for (size_t i = 0; i < FreePages.size(); i++) {
		PagesDescribers.insert({ FreePages[i], { PageState::Free, 0, nullptr, 0 } });
	}
};

void Allocator::define_class_describers() {
	for (int i = CLASS_MIN; pow(2, i) <= PAGE_SIZE / 2; i++) {
		classDescFreeBlock.insert({ i,{} });
	};
};
void Allocator::mem_show() {

	for (const pair<void*, PageDescriber> page : PagesDescribers) {
		cout << "========================================================\n";
		cout << "Adress of list: ";
		cout << page.first << "\n";
		if (page.second.pageState == PageState::Free) {
			cout << "State: Free\n";
		} 
		if (page.second.pageState == PageState::Divided_Into_Blocks) {
			cout << "State: Divided Into Blocks; Class: " << page.second.blockClass;
			cout << "; First free block: " << page.second.firstFreeBlock;
			cout << "; Free blocks: " << page.second.freeBlocks;
			cout << "\n";
		}
		if(page.second.pageState == PageState::Filled_multipage_block)
		{
			cout << "State: Filled multipage block; Size: " << page.second.blockClass;
			cout << "\n";
		}
		
	};
	cout << "========================================================\n";
	cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n";
};

void Allocator::change_FreePages() {
	FreePages.clear();
	for (map <void*, PageDescriber> ::iterator it = PagesDescribers.begin(); it != PagesDescribers.end(); it++) {
		if (it->second.pageState == PageState::Free) {
			FreePages.push_back(it->first);
		};
	};
};

int Allocator::count_class_block(size_t size) {
	int c = CLASS_MIN;
	for (; pow(2, c) < size; c++);
	return c;
};


void Allocator::divided_page_into_blocks(int blockClass, void* page) {


	int blockSize = (int)pow(2, blockClass);

	PagesDescribers[page].pageState = PageState::Divided_Into_Blocks;
	PagesDescribers[page].blockClass = blockClass;
	PagesDescribers[page].freeBlocks = PAGE_SIZE / blockSize;
	PagesDescribers[page].firstFreeBlock = (int*)page;

	int* p = (int*)page;

	int* nextp;

	for (int i = 0; i < PagesDescribers[page].freeBlocks; i++) {

		nextp = (int*)(p + blockSize/4);
		*p = (int)nextp;

		p += blockSize / 4;
	}

	classDescFreeBlock[blockClass].push_back(page);

};

void* Allocator::get_free_block(void* page) {
	void* p = (void*)PagesDescribers[page].firstFreeBlock;

	PagesDescribers[page].firstFreeBlock = (int*)*PagesDescribers[page].firstFreeBlock;
	--PagesDescribers[page].freeBlocks;
	return p;
};

void* Allocator::flag_multipages(void* start, int pages) {


	int* p = (int*)start;
	for (int i = 0; i < pages; i++) {
		
		PagesDescribers[p].pageState = PageState::Filled_multipage_block;
		PagesDescribers[p].blockClass = pages;
		p += PAGE_SIZE / 4;
	};
	change_FreePages();
	return start;
};

void* Allocator::multipages_block(size_t size) {

	int needList = 0;
	int temps = size;
	while (true) {
		if (temps > 0) temps -= PAGE_SIZE;
		else {
			break;
		}
		needList++;
	}
	if (needList > FreePages.size()) return nullptr;
	int* p;
	map <void*, PageDescriber>::iterator iter;

	for (int i = 0; i < FreePages.size(); i++) {

		p = (int*)FreePages[i];
		for (int j = 0; j < needList; j++) {

			iter = PagesDescribers.find(p);
			if (iter == PagesDescribers.end()) break;

			if (iter->second.pageState == PageState::Free) {
				if (j + 1 == needList) {
					return flag_multipages(FreePages[i], needList);
				}
				p += PAGE_SIZE / 4;
			}
			else break;
		}
	}

	return nullptr;
};

void* Allocator::mem_alloc(size_t size) {
		
	if (size < PAGE_SIZE / 2) {
		int blockClass = count_class_block(size);

		if (classDescFreeBlock[blockClass].size() == 0) {

			divided_page_into_blocks(blockClass,FreePages[0]);

			FreePages.erase(FreePages.begin());

			return get_free_block(classDescFreeBlock[blockClass][0]);
		}
		for (int i = 0; i < classDescFreeBlock[blockClass].size(); i++) {
			if(PagesDescribers[classDescFreeBlock[blockClass][i]].freeBlocks > 1) return get_free_block(classDescFreeBlock[blockClass][i]);
		}

		divided_page_into_blocks(blockClass, FreePages[0]);
		FreePages.erase(FreePages.begin());
		return get_free_block(classDescFreeBlock[blockClass][classDescFreeBlock[blockClass].size() - 1]);
	}
	else
	{
		if (FreePages.size() == 0) {
			return nullptr;
		}

		return multipages_block(size);
	}
};

void Allocator::free_small_block(void* page, void* addr) {

	int* p = PagesDescribers[page].firstFreeBlock;
	int* temp = p;

	if (addr < p) {
		temp = (int*)addr;
		*temp = (int)p;
		PagesDescribers[page].firstFreeBlock = (int*)addr;
		PagesDescribers[page].freeBlocks++;
	}
	else {
		while (true)
		{
			if (addr < p) {
				*temp = (int)addr;
				temp = (int*)addr;
				*temp = (int)p;
				PagesDescribers[page].freeBlocks++;
				break;
			}
			temp = p;
			p = (int*)*p;
		};
	}

	if (PagesDescribers[page].freeBlocks == PAGE_SIZE/(int)pow(2, PagesDescribers[page].blockClass)) {
		PagesDescribers[page].pageState = PageState::Free;
		for (int i = 0; i < classDescFreeBlock[PagesDescribers[page].blockClass].size(); i++) {
			if (classDescFreeBlock[PagesDescribers[page].blockClass][i] == page) {
				classDescFreeBlock[PagesDescribers[page].blockClass].erase(classDescFreeBlock[PagesDescribers[page].blockClass].begin() + i);
			};
		};
		PagesDescribers[page].firstFreeBlock = nullptr;
		PagesDescribers[page].blockClass = 0;
		PagesDescribers[page].freeBlocks = 0;
		update_freePages();
	};
	
};

void* Allocator::fing_page_with_block(void* addr) {

	map <void*, PageDescriber>::iterator iter;
	iter = PagesDescribers.begin();
	for (; iter != PagesDescribers.end(); iter++) {
		if (addr < iter->first){
			iter--;
			return iter->first;
		}
	};
	
};

void Allocator::free_multiblock(void* page) {
	
	int leng = PagesDescribers[page].blockClass;
	int* p = (int*)page;
	for (int i = 0; i < leng; i++) {
		if (PagesDescribers[p].pageState == PageState::Filled_multipage_block) {
			PagesDescribers[p].blockClass = 0;
			PagesDescribers[p].pageState = PageState::Free;
		}
		p += PAGE_SIZE / 4;
	}

};

void Allocator::update_freePages() {
	FreePages.clear();
	map <void*, PageDescriber>::iterator iter;
	iter = PagesDescribers.begin();
	for (; iter != PagesDescribers.end(); iter++) {
		if (iter->second.pageState == PageState::Free) {
			FreePages.push_back(iter->first);
		}
	};
};


void Allocator::mem_free(void* addr) {

	map <void*, PageDescriber>::iterator iter;
	iter = PagesDescribers.find(addr);
	if (iter == PagesDescribers.end()) {
		free_small_block(fing_page_with_block(addr), addr);
	}
	else if (iter->second.pageState == PageState::Divided_Into_Blocks) {
		free_small_block(iter->first, addr);
	}
	else if (iter->second.pageState == PageState::Filled_multipage_block) {
		free_multiblock(addr);
		update_freePages();
	};
	
};

void Allocator::mem_move(void* from, void* to, int length) {
	char *ptr1 = (char*)from, *ptr2 = (char* )to;
	for (int i = 0; i < length; i++) {
		*ptr2 = *ptr1;
		ptr1++;
		ptr2++;
	}
};

void Allocator::mem_copy(void* from, void* to) {
	
	int firstSize = 0, secondSize = 0;

	for (map <void*, PageDescriber>::iterator iter = PagesDescribers.begin(); iter != PagesDescribers.end(); iter++) {
		if (from < iter->first && firstSize == 0) {
			iter--;
			if (iter->second.pageState == PageState::Divided_Into_Blocks) {
				firstSize = pow(2, iter->second.blockClass);
			}
			else if (iter->second.pageState == PageState::Filled_multipage_block) {
				firstSize = PAGE_SIZE * iter->second.blockClass;
			}
			iter++;
		};
		if (to < iter->first && secondSize == 0) {
			iter--;
			if (iter->second.pageState == PageState::Divided_Into_Blocks) {
				secondSize = pow(2, iter->second.blockClass);
			}
			else if (iter->second.pageState == PageState::Filled_multipage_block) {
				secondSize = PAGE_SIZE * iter->second.blockClass;
			}
			iter++;
		};
		if (secondSize != 0 && firstSize != 0) {
			break;
		}
	};
	mem_move(from ,to , firstSize < secondSize ? firstSize : secondSize);

};

void* Allocator::mem_realloc(void* addr, size_t size) {

	if (addr == nullptr || addr > memory + MEMORY_SIZE/4) {
		return mem_alloc(size);
	}
	else {
		void* p = mem_alloc(size);
		mem_copy(addr, p);

		mem_free(addr);

		return p;
	};
};