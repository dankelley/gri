// make 8bit or 16bit data for testing gri

#include <stdio.h>
int main() 
{
    for (int i = 0; i < 5; i++) {
	for (int j = 0; j < 5; j++) {
	    unsigned short int v = (unsigned short int)((i + 2 * j) * 255.0 / 12.0);
	    //unsigned char v = (unsigned char)((i + 2 * j) * 255.0 / 12.0);
	    fwrite(&v, sizeof(v), 1, stdout);
	    fprintf(stderr, "%d ", v);
	}
	fprintf(stderr, "\n");
    }
}
