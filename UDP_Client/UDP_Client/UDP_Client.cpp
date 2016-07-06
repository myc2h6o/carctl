#include "Sender.h"

const int size = 1027;

int main() {
	char c[size];
	for (int i = 0; i < size; ++i) {
		c[i] = size % 512;
	}
	Sender::send(c, size);
}