#ifndef ORDEREDPOOL_H_
#define ORDEREDPOOL_H_

class OrderedPool;

bool IndexedSort(const std::pair<int,int> &lhs, const std::pair<int,int> &rhs);

class OrderedPool : public EntityPool {
protected:
	std::vector< std::pair<int,int> > m_indices;
public:
	OrderedPool();
	OrderedPool(const OrderedPool &rhs);
	~OrderedPool();
	void DrawAll(cScreen *screen,Printer *printer); //Draw based on indices
	void Push(Entity *e, int order);
	void Push(Entity *e);
};

#endif