#include "Bmp.h"
#include<cstdio>
#include <cstring>
#include <cmath>

bool loadImage(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		return false;
	}
	fclose(file);
	return true;
}