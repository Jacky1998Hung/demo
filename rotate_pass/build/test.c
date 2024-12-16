#include <stdio.h>
int main() {
	int ret = 0;
	for( int j = 0;j < 4; j++ ) {
		ret+= j; 
	}
	printf("%d", ret);
	return 0;
}
