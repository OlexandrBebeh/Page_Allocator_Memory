#include <iostream>
#include "Allocator.h"
using namespace std;

void test() {
	Allocator a;
	a.mem_show();
	int* b1 = (int*)a.mem_alloc(20);
	int* b2 = (int*)a.mem_alloc(200);
	int* b3 = (int*)a.mem_alloc(4);
	int* b4 = (int*)a.mem_alloc(4);
	int* b5 = (int*)a.mem_alloc(4);


	a.mem_show();
	int* b6 = (int*)a.mem_alloc(500);
	int* b7 = (int*)a.mem_alloc(1500);
	int* b8 = (int*)a.mem_alloc(1500);
	a.mem_show();

	a.mem_free(b4);
	a.mem_free(b5);
	a.mem_free(b3);
	a.mem_show();

	a.mem_free(b6);
	a.mem_free(b7);
	a.mem_show();

	*b2 = 2024;
	int* b9 = (int*)a.mem_realloc(b2, 4);
	cout << "///////////\n";
	cout << *b9 << "\n";
	cout << "///////////\n";
	int* b10 = (int*)a.mem_realloc(b9, 300);
	a.mem_show();
	
};


int main()
{
	test();
	return 0;
}