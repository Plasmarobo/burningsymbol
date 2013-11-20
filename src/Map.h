#ifndef MAP_H_
#define MAP_H_

#include "Entities.h"
#include "EntityPool.h"
#include "State.h"
#include "Tile.h"
#include "Trigger.h"
#include "Unit.h"
#include "OrderedPool.h"
#include "Status.h"
#include "astar.h"

struct Coord;
struct Glide;
class Camera;
class Map;

struct Coord{
	unsigned int x;
	unsigned int y;
	Coord();
	Coord(const Coord &rhs);
};

struct Glide{
	int tx;
	int ty;
	int ox;
	int oy;
	bool stop_able;
	double start;
	double time;
	bool done;
	bool started;
	Glide();
	Glide(int x, int y, unsigned int t, bool s);
	Glide(const Glide &rhs);
	void operator=(const Glide &rhs);
	//void Run(unsigned int now);
	//void Update(unsigned int now);
	int X(double now);
	int Y(double now);
	void Start(unsigned int now, int x, int y);
	void Stop(double now);
};

class Camera  : public Mutexable {
protected:
	std::queue<Glide> m_path;
	int m_x;
	int m_y;
	int m_x_min;
	int m_y_min;
	int m_x_max;
	int m_y_max;
public:
	Camera();
	Camera(int x, int y);
	Camera(const Camera &rhs);
	void operator=(const Camera &rhs);
	void Stop(double now);
	void AddGlide(Glide &g);
	int X();
	int Y();
	void Set(int x, int y);
	void Update(unsigned int now);
	void SetLimits(int minx, int miny, int maxx, int maxy);
	bool Gliding();

};

enum PathDir{ //Map to path animations TODO: Scripting override
	drc = 0, //Down-and-right
	dlc, //Down-and-left
	up, //Up terminator
	urc, //Up-and-right 
	ulc, //Up-and-left
	ud, //Up-and-down
	lf, //Left terminator
	lr, //Left-and-right
	ri, //Right terminator
	dn //Down terminator
};
namespace PathType
{
enum PathType{
	pt_valid = 0,
	pt_attack,
	pt_effect,
	pt_misc
};
}

class Map : public State{
protected:
	//EntityPool *res;
	Tile_Ref ***tiles; //2d array of pointers (Rather than data)
	Trigger ***triggers; //2d complimentary array of triggers
	unsigned int t_width; //Tile width
	unsigned int t_height; //Tile height
	unsigned int m_width; //Map width in tiles
	unsigned int m_height; //Map height in tiles
	unsigned int m_c_x; // Cursor X
	unsigned int m_c_y; // Cursor Y
	unsigned int m_cam_edge; //How many tiles from the edge
	Camera m_cam;
	Tile_Ref *m_cursor_ref; //The tile class allows for seamless map integration
	Tile_Ref *m_overlay_ref; 
	OrderedPool *m_overlay_pool;
	Tile_Ref *m_path_ref;
	OrderedPool *m_path_pool;
	unsigned int m_cursor_last_y;
	unsigned int m_cursor_last_x;
	std::vector<bool> m_valids;

	std::vector<InstUnit*> m_party;
	std::vector<InstUnit*> m_allies;
	std::vector<InstUnit*> m_enemies;
	std::vector<InstUnit*> m_others;
	
	Tile_Ref *m_c_t;
	InstUnit *m_c_u;
	
	InstUnit *m_target; //Command mode target
	UnitWindow *m_unitwnd; //Unit info display
	TileWindow *m_tilewnd; //Tile info display
	ObjectiveWindow *m_objwnd; //Objective info display
	
	Coord *m_placement;
	unsigned int m_placement_count;
	std::string m_invoke;

	std::map<int, std::string> m_mode_src;
	
	InstUnit *m_defer_target;

	MenuItem *m_bt;
	//Invocations and context
	void _UpdateTiles();
	int TranslateX(int x); //Map coords to abs cords
	int TranslateY(int y);
	void _CheckCursor();
	void _HandleCam(unsigned int now);
	void _CenterCam(unsigned int now);
	
	bool _IsPlayer(InstUnit *target);

	void _SelectMode(Input::IEvent &e);
	void _CommandMode(Input::IEvent &e);
	void _ActionMode();
	void _CommonInputProc(Input::IEvent &e);
	void _ScriptMode();
	int _mode_id;

	unsigned int _cs; //Time to suspend map input handling for
	astar::path_entry *_BuildPath(int sx, int sy, int dx, int dy, int mov, int type);
	//void _ConstructPath(node *node_list);
public:
	Map();
	Map(const Map &rhs);
	~Map();
	virtual void Update();
	virtual void Idle();
	void Build(int tw, int th, int mw, int mh);
	void UpdateTileRef(int x, int y, Tile_Ref *data);
	void Create(ResourceManager *res, bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::string invoke, std::vector<State*> *stack, Tile_Ref ***t,unsigned int tw,unsigned int th,unsigned int w, unsigned int h, Coord *pls, unsigned int p);
	
	void MoveCursor(int x, int y);
	void SetCursor(unsigned int x, unsigned int y);
	Tile_Ref *GetCursor();
	Tile_Ref *GetOverlay();
	unsigned int CursorX();
	unsigned int CursorY();

	unsigned int GetWidth();
	unsigned int GetHeight();
	unsigned int GetTileW();
	unsigned int GetTileH();

	unsigned int InsertOther(DefUnit *ref, int x, int y);
	unsigned int InsertParty(DefUnit *ref, int x, int y);
	unsigned int InsertEnemy(DefUnit *ref, int x, int y);
	unsigned int InsertAlly(DefUnit *ref, int x, int y);
	InstUnit* GetOther(std::string name);
	InstUnit* GetOther(unsigned int index);
	InstUnit* GetParty(std::string name);
	InstUnit* GetParty(unsigned int index);
	InstUnit* GetEnemy(std::string name);
	InstUnit* GetEnemy(unsigned int index);
	InstUnit* GetAlly(std::string name);
	InstUnit* GetAlly(unsigned int index);

	void ShiftCam(int x, int y); //Move the camera
	void GlideCam(int x, int y, unsigned int time);
	void GlideCam(Glide &g);
	void CenterCamOn(int x, int y);
	void StopCam();
	Camera *GetCam();
	
	void SetCursorEnt(Entity *e);
	void SetPathEnt(Entity *e);
	void SetPathTile(Tile_Ref *r);
	void SetOverlayEnt(Entity *e);
	void SetOverlayTile(Tile_Ref *r);
	void SetTiles(Tile_Ref ***t, ResourceManager *res);// 2d array of pointers
	Tile_Ref* Select(int x, int y);
	Tile_Ref* Overlay(int x, int y, int mode);
	Tile_Ref* Path(int x, int y, int px, int py, int nx, int ny);
	Tile_Ref* GetTile(int x, int y);
	InstUnit* GetUnitAt(int x, int y);
	void RemoveUnitAt(int x, int y);
	
	void Free();
	virtual void Run();

	void SetTrigger(Trigger *t, int x, int y);
	Trigger *GetTrigger(int x, int y);
	void ActivateTrigger(int x, int y);

	void SetPlace(Coord *pls, unsigned int p);
	unsigned int PlaceCount();
	Coord GetPlacement(int i);
	bool SetCommandMode(InstUnit *target);
	bool TryCommandTile(int x, int y);
	void SetSelectMode();
	void SetActionMode(InstUnit *target);
	void SetButtonTemplate(Entity *e);

	int GetScriptMode();
	void SetModeScript(int i, std::string str);
	std::string GetModeScript(int i);
	void SetScriptMode(int i);

	void SetAsTarget(InstUnit *u);

};


//Unity/Entity manipulation functions



#endif