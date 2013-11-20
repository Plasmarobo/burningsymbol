#include "astar.h"
/** {{{ http://code.activestate.com/recipes/577457/ (r4) */
// Astar.cpp
// http://en.wikipedia.org/wiki/A*
// Modified and adapted by Austen Higgins-Cassidy, 10.02.2012
#include <queue>
#include <math.h>

astar::node::node(int xp, int yp, int d, int p) 
            {xPos=xp; yPos=yp; level=d; priority=p;}
    
        int astar::node::getxPos() const {return xPos;}
        int astar::node::getyPos() const {return yPos;}
        int astar::node::getLevel() const {return level;}
        int astar::node::getPriority() const {return priority;}

        void astar::node::updatePriority(const int & xDest, const int & yDest)
        {
             priority=level+estimate(xDest, yDest)*10; //A*
        }

        // give better priority to going strait instead of diagonally
        void astar::node::nextLevel(const int & i) // i: direction
        {
             level+=(dir==8?(i%2==0?10:14):10);
        }
        
        // Estimation function for the remaining distance to the goal.
        const int & astar::node::estimate(const int & xDest, const int & yDest) const
        {
            static int xd, yd, d;
            xd=xDest-xPos;
            yd=yDest-yPos;         

            // Euclidian Distance
            d=static_cast<int>(sqrt((float)(xd*xd+yd*yd)));

            // Manhattan distance
            //d=abs(xd)+abs(yd);
            
            // Chebyshev distance
            //d=max(abs(xd), abs(yd));

            return(d);
        }

// Determine priority (in the priority queue)
bool astar::operator<(const node & a, const node & b)
{
  return a.getPriority() > b.getPriority();
}

astar::path_entry::path_entry(int a, int b, path_entry *n = NULL) : x(a), y(b), next(n) {}

// A-star algorithm.
// The route returned is a string of direction digits.
astar::path_entry* astar::pathFind(int **map, int n, int m, const int & xStart, const int & yStart, const int & xFinish, const int & yFinish )
{
	int **closed_nodes_map;
	int **open_nodes_map;
	int **dir_map; // map of directions
	closed_nodes_map = new int*[n];
	open_nodes_map = new int*[n];
	dir_map = new int*[n];
	for(int i = 0; i < n; ++i){
		closed_nodes_map[i] = new int[m]; // map of closed (tried-out) nodes
		open_nodes_map[i] = new int[m]; // map of open (not-yet-tried) nodes
		dir_map[i] = new int[m];
	}
    std::priority_queue<astar::node> pq[2]; // list of open (not-yet-tried) nodes
    int pqi; // pq index
    astar::node* n0;
    astar::node* m0;
    int i, j, x, y, xdx, ydy;
//    char c;
    pqi=0;

    // reset the node maps
    for(y=0;y<m;++y)
    {
        for(x=0;x<n;++x)
        {
            closed_nodes_map[x][y]=0;
            open_nodes_map[x][y]=0;
        }
    }

    // create the start node and push into list of open nodes
    n0=new node(xStart, yStart, 0, 0);
    n0->updatePriority(xFinish, yFinish);
    pq[pqi].push(*n0);
    open_nodes_map[x-1][y-1]=n0->getPriority(); // mark it on the open nodes map

    // A* search
    while(!pq[pqi].empty())
    {
        // get the current node w/ the highest priority
        // from the list of open nodes
        n0=new node( pq[pqi].top().getxPos(), pq[pqi].top().getyPos(), 
                     pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

        x=n0->getxPos(); y=n0->getyPos();

        pq[pqi].pop(); // remove the node from the open list
        open_nodes_map[x][y]=0;
        // mark it on the closed nodes map
        closed_nodes_map[x][y]=1;

        // quit searching when the goal state is reached
        //if((*n0).estimate(xFinish, yFinish) == 0)
        if(x==xFinish && y==yFinish) 
        {
            // generate the path from finish to start
            // by following the directions
            path_entry *ptr = NULL;
            path_entry *ptr_last = NULL;
            while(!(x==xStart && y==yStart))
            {
                j=dir_map[x][y];
                //c='0'+(j+dir/2)%dir;
                //path=c+path;
                ptr = new path_entry(x, y, ptr_last); //end
                ptr->dir = j;
                x+=dx[j];
                y+=dy[j];
                ptr_last = ptr;
            }
			//Copy the first node
			ptr = new path_entry(x, y, ptr_last);
			ptr->dir = dir_map[x][y];

            // garbage collection
            delete n0;
            // empty the leftover nodes
            while(!pq[pqi].empty()) pq[pqi].pop();    
            for(int i = 0; i < n; ++i){
				delete [] closed_nodes_map[i];// = new int[m]; // map of closed (tried-out) nodes
				delete [] open_nodes_map[i];// = new int[m]; // map of open (not-yet-tried) nodes
				delete [] dir_map[i];// = new int[m];
			}       
			delete [] closed_nodes_map;
			delete [] open_nodes_map;
			delete [] dir_map;
            return ptr;
        }

        // generate moves (child nodes) in all possible directions
        for(i=0;i<dir;i++)
        {
            xdx=x+dx[i]; ydy=y+dy[i];

            if(!(xdx<0 || xdx>n-1 || ydy<0 || ydy>m-1 || map[xdx][ydy]==1 
                || closed_nodes_map[xdx][ydy]==1))
            {
                // generate a child node
                m0=new node( xdx, ydy, n0->getLevel(), 
                             n0->getPriority());
                m0->nextLevel(i);
                m0->updatePriority(xFinish, yFinish);

                // if it is not in the open list then add into that
                if(open_nodes_map[xdx][ydy]==0)
                {
                    open_nodes_map[xdx][ydy]=m0->getPriority();
                    pq[pqi].push(*m0);
                    // mark its parent node direction
                    dir_map[xdx][ydy]=(i+dir/2)%dir;
                }
                else if(open_nodes_map[xdx][ydy]>m0->getPriority())
                {
                    // update the priority info
                    open_nodes_map[xdx][ydy]=m0->getPriority();
                    // update the parent direction info
                    dir_map[xdx][ydy]=(i+dir/2)%dir;

                    // replace the node
                    // by emptying one pq to the other one
                    // except the node to be replaced will be ignored
                    // and the new node will be pushed in instead
                    while(!(pq[pqi].top().getxPos()==xdx && 
                           pq[pqi].top().getyPos()==ydy))
                    {                
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();       
                    }
                    pq[pqi].pop(); // remove the wanted node
                    
                    // empty the larger size pq to the smaller one
                    if(pq[pqi].size()>pq[1-pqi].size()) pqi=1-pqi;
                    while(!pq[pqi].empty())
                    {                
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();       
                    }
                    pqi=1-pqi;
                    pq[pqi].push(*m0); // add the better node instead
                }
                else delete m0; // garbage collection
            }
        }
        delete n0; // garbage collection
    }
    for(int i = 0; i < n; ++i){
			delete [] closed_nodes_map[i];// = new int[m]; // map of closed (tried-out) nodes
			delete [] open_nodes_map[i];// = new int[m]; // map of open (not-yet-tried) nodes
			delete [] dir_map[i];// = new int[m];
		}       
	delete [] closed_nodes_map;
	delete [] open_nodes_map;
	delete [] dir_map;
    return NULL; // no route found
}

/** end of http://code.activestate.com/recipes/577457/ }}} */