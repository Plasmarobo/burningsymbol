#ifndef A_STAR_H
#define A_STAR_H
/** {{{ http://code.activestate.com/recipes/577457/ (r4) */
// Astar.cpp
// http://en.wikipedia.org/wiki/A*
//Modified and adapted by Austen Higgins-Cassidy, 10.02.2012
// Compiler: Dev-C++ 4.9.9.2
// FB - 201012256
namespace astar {

static const int dir=4; // number of possible directions to go at any position
// if dir==4
static int dx[dir]={1, 0, -1, 0};
static int dy[dir]={0, 1, 0, -1};

class node
{
    // current position
    int xPos;
    int yPos;
    // total distance already travelled to reach the node
    int level;
    // priority=level+remaining distance estimate
    int priority;  // smaller: higher priority

    public:
        node(int xp, int yp, int d, int p) ;
    
        int getxPos() const;
        int getyPos() const;
        int getLevel() const;
        int getPriority() const;

        void updatePriority(const int & xDest, const int & yDest);

        // give better priority to going strait instead of diagonally
        void nextLevel(const int & i);
        
        // Estimation function for the remaining distance to the goal.
        const int & estimate(const int & xDest, const int & yDest) const;
};

// Determine priority (in the priority queue)
bool operator<(const node & a, const node & b);

struct path_entry {
	int x;
	int y;
	int dir;
	path_entry *next;
	path_entry(int a, int b, path_entry *n);
};

path_entry* pathFind(int **map, int n, int m, const int & xStart, const int &yStart, const int & xFinish, const int &yFinish);

}

#endif