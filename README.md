# Page Memory Allocator

## Page Memory Allocator on C++ 

Realization of memory allocator using pages. All pages have same size. Pages could be in one of three state: free, devide into block, multipage block. Free pages could be used to be .Page devided into equal blocks. Size of block calculate **2^n**, where **n** starts from **CLASS_MIN**. 
All pages have describers.Describers consist of state of page, class of blocks, amount of free blocks and pointer to first free block.
When page devides into blocks every block contain pointer to next free block.

Constants:

**MEMORY_SIZE** - total size of memory we use.
 
**PAGE_SIZE** - size of page in which devided total memory.

**CLASS_MIN** - minimun digit for degree of 2. Another words 2^CLASS_MIN minimun size of block.

Functions:

`void* mem_alloc(size_t size)` – allocate memory of given size. There are three ways in which memory allocate:
If `size` is less than half of PAGE_SIZE gives memory from page which gevides into blocks where enough size. 
If `size` is bigger than half of PAGE_SIZE gives amout of pages you need.
If there not enough of memory return `nullptr`.
 
`void mem_free(void* addr)` – free momory by given link in allocator. 
If link is block mark this block as free and if all blocks in page free mark this page as free.
If link is multipage memory mark all pages as free.
Pages which become free pushs to list of free pages.
	
`void* mem_realloc(void* addr, size_t size)` – takes new memory with help of function `mem_alloc` of `size`. Then copy data from old ref if enough space in it, if not copy what can. After that free memory in `addr`.
If `addr = nullptr` work as `mem_alloc`.

## Usege

To use the algorithm, open the file in any IDE with C++ support.
Describe your use cases in main() function or implement it in separate function, then calling it in main() function.

## Examples

> I'm use function `mem_show()` to demonstrate you state of blocks.

Let's start with simple example, like creation of allocator.

Code

```
	Allocator a;
	
	a.mem_show();
```

Output

![example 1](/images/example1.png)

Example when allocate blocks of small size.

Code

```
	int* b1 = (int*)a.mem_alloc(20);
	int* b2 = (int*)a.mem_alloc(200);
	int* b3 = (int*)a.mem_alloc(4);
	int* b4 = (int*)a.mem_alloc(4);
	int* b5 = (int*)a.mem_alloc(4);
	
	a.mem_show();
```

Output

![example 2](/images/example2.png)

Example of allocating large blocks

Code

```
	int* b6 = (int*)a.mem_alloc(500);
	int* b7 = (int*)a.mem_alloc(1500);
	int* b8 = (int*)a.mem_alloc(1500);
	a.mem_show();
```
Output

![example 3](/images/example3.png)

You can see that for last block wasn't give memory.

Let's free all memory in third page.

Code

```
	a.mem_free(b4);
	a.mem_free(b5);
	a.mem_free(b3);
	a.mem_show();
```
Output

![example 4](/images/example4.png)

After all that this page mark as free.

Next what we will free are multipage blocks.

Code

```
	a.mem_free(b6);
	a.mem_free(b7);
	a.mem_show();
```
Output

![example 5](/images/example5.png)

Now we will realloc some memory to less size. Before that we will write value to this pointer.

Code

```
	*b2 = 2024;
	int* b9 = (int*)a.mem_realloc(b2, 4);
	a.mem_show();
```
Output

![example 6](/images/example6.png)

As you can see there value was copied.

Code

```
	cout << "///////////\n";
	cout << *b9 << "\n";
	cout << "///////////\n";
```
Output

![example 7](/images/example7.png)

Now we realloc to bigger size.

Code

```
	int* b10 = (int*)a.mem_realloc(b9, 300);
	a.mem_show();
```
Output

![example 8](/images/example8.png)


