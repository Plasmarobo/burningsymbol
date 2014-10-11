#include "Map.h"


int main(int argc, char **argv){
	node ***graph;
	int length = 0;
	std::priority_queue<node*, std::vector<node*>, node_comp> open_set;
	if((abs(dx-sx)+abs(dy-sy)) > mov)
		return NULL;
	else {
		graph = new node**[(2*mov)+1];
		for(int i = 0; i < (2*mov)+1; ++i)
			graph[i] = new node*[(2*mov)+1];
	}
	//We now have an empty map that contains more than all the possible tiles
	//After this point we assume that every point is valid
	//populate initial nodes
	for(int y = -mov; y <= mov; ++y)
		for(int x = -mov; x <= mov; ++x){
			graph[mov+y][mov+x] = NULL;
			if(sx+x < 0 || sx+x >= t_width) continue;
			if(sy+y < 0 || sy+y >= t_height) continue;
			if(tiles[sy+y][sx+x]->TileInfo()->GetTera() > type) continue; //Leave null holes where impassable areas are
			if(GetUnitAt(sx+x, sy+y) != NULL) continue; //reject tiles with enemy units
			graph[mov+y][mov+x] = new node(sx+x, sy+y, abs(dx-x)+abs(dy-y)+1, 99);
			graph[mov+y][mov+x]->a = false;
		}
		graph[mov][mov] = new node(sx, sy, 0, abs(dx-sx)+abs(dy-sy));
	//populate neighbors
	for(int y = -mov; y <= mov; ++y)
		for(int x = -mov; x <= mov; ++x){
			if(graph[mov+y][mov+x] == NULL) continue;
			if(x != -mov) graph[mov+y][mov+x]->left = graph[mov+y][mov+x-1];
			if(x != mov) graph[mov+y][mov+x]->right = graph[mov+y][mov+x+1];
			if(y != -mov) graph[mov+y][mov+x]->top = graph[mov+y-1][mov+x];
			if(y != mov) graph[mov+y][mov+x]->bottom = graph[mov+y+1][mov+x];
		}
	//we now have a neighbor-complete graph of possible moves
	graph[mov][mov]->z = 0;
	graph[mov][mov]->a = true;
	graph[mov][mov]->w = abs(dx-sx)+abs(dy-sy);
	open_set.push(graph[mov][mov]); //Priority queue
	node *current;
	while(!open_set.empty()){
		current = open_set.top();
		//if(current->v) break;
		if(current->x == dx && current->y == dy) {
			node *node_list = new node[mov];
			//reconstruct and return path
			for(int i = 0; i < mov; ++i){
				length++;
				node_list[i] = *current;
				node_list[i].v = false;
				if(current == graph[mov][mov]) {node_list[i].v = true; break;}
				if(current->pt == NULL) break;
				current = current->pt;
			}
			for(int i = 0; i < (2*mov)+1; ++i){
				for(int j = 0; j < (2*mov)+1; ++j)
					delete graph[i][j];
				delete [] graph[i];
			}
			delete [] graph;
			return node_list;
		}
		open_set.pop();
		current->v = true;
		node *n = NULL;
		int tg = 0;

			for(int i = 0; i < 4; ++i){
				if(i == 0) n = current->left;
				if(i == 1) n = current->right;
				if(i == 2) n = current->top;
				if(i == 3) n = current->bottom;
				if(n == NULL) continue;
				//if(n->v) continue;
				tg = current->z + 1;
				if(tg < n->z || !n->a){
					if(tg < n->z){
						n->pt = current;
						n->z = tg;
						n->w = n->z + abs(dx-n->x)+abs(dy-n->y);
					}
					if(!n->a){
						n->a = true;
						if(!n->v)
							open_set.push(n);
					}
				}
			}
		
	}
	for(int i = 0; i < (2*mov)+1; ++i){
				for(int j = 0; j < (2*mov)+1; ++j)
					delete graph[i][j];
				delete [] graph[i];
			}
			delete [] graph;
	return NULL;

	return 0;
}