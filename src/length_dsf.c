#include <assert.h>
#include "length_dsf.h"

// API
void adsf_assert_consistency(int *t, int n, int* size)
{
	assert(n > 0);
	assert(t);
	for (int i = 0; i < n; i++) {
		assert(t[i] >= 0);
		assert(t[i] < n);
		assert(size[t[i]] >= size[i]);
	}
}

// API
void adsf_begin(int *t, int n, int *size)
{
	for (int i = 0; i < n; i++) {
		t[i] = i;
		size[i] = 1;
	}
}

// API
int adsf_find(int *t, int n, int a)
{
	assert(a >= 0 && a < n);
	if (a != t[a])
		t[a] = adsf_find(t, n, t[a]);
	return t[a];
}

static int adsf_make_link(int *t, int a, int b, int* size)
{
	if (a < b) {
		t[b] = a;
		size[a] += size[b];
		return a;
	} else {
		t[a] = b;
		size[b] += size[a];
		return b;
	}
}

// API
int adsf_union(int *t, int n, int a, int b, int* size)
{
	assert(a >= 0 && a < n);
	assert(b >= 0 && b < n);
	a = adsf_find(t, n, a);
	b = adsf_find(t, n, b);
	int c = a;
	if (a != b)
		c = adsf_make_link(t, a, b,size);
	return c;
}

// API
//int adsf_number_of_classes(int *t, int n)
//{
//	adsf_assert_consistency(t, n);
//	error("not yet implemented");
//	return -1;
//}
