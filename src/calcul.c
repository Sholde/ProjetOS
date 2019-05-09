#include "navalmap.h"
#include "nm_rect.h"

coord_t to_coord(int x, int y) {
	coord_t tmp;
	tmp.x = x;
	tmp.y = y;
	return tmp;
}

int absolu(int x) {
	x = x<0 ? -x : x;
	return x;
}

int calcul_distance(
	coord_t a,
	coord_t b) {
	
	int x, y;
	x = a.x - b.x;
	y = a.y - b.y;
	x = absolu(x);
	y = absolu(y);
	int distance =  x + y;
	
	return distance;
}

int puissance (int n, int puissance) {
	int res = 1;
	for(int i = 0; i < puissance; i++)
	{
		res *= n;
	}
	return res;
}
