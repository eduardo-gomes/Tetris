#include <DuEngine/DuEngine.hpp>
//constants
constexpr int RIGHT = 1, LEFT = -1;
constexpr unsigned int TILES_COLUMNS = 15, TILES_ROWS = 5;
constexpr float TILES_V_OFSSET = 0.2, TILES_ROWS_SCREEN_PERCENTAGE = 0.35;	//SCREENSIZE 2 = all screen
constexpr float BORDER_ABSOLUTE = 0.015, TILES_BORDER_ABSOLUTE = 0.005;

constexpr float ASPECT_RATIO = 16.0 / 9;
constexpr float BAR_H_SIZE = 0.7, BAR_V_SIZE = 0.0125;
constexpr float BAR_SPEED = 0.03, BAR_V_OFFSET = 0.95, BAR_Y_POS = (-1 + BORDER_ABSOLUTE) * BAR_V_OFFSET - BAR_V_SIZE / 2;

constexpr float GAME_AREA_H = 2 * (ASPECT_RATIO - (ASPECT_RATIO * BORDER_ABSOLUTE)), GAME_AREA_V = 2 * (1 - (1 * BORDER_ABSOLUTE));
constexpr float TILES_H_SIZE = GAME_AREA_H / TILES_COLUMNS - (TILES_BORDER_ABSOLUTE * (TILES_COLUMNS + 1));	 //border | tile | border | tile | border
constexpr float TILES_V_SIZE = GAME_AREA_V * TILES_ROWS_SCREEN_PERCENTAGE / TILES_ROWS - (TILES_BORDER_ABSOLUTE * (TILES_ROWS));

class breakout : public scene::BaseScene {
   public:
	breakout();
	~breakout();
	void Update(int64_t);
	void Render();
	void RenderGUI();

   private:
	unsigned int score;
	class pbola;
	void colide(pbola &);
	class pbola {
	   public:
		float x = 0, y = 0, angle, speed = 1.0, diameter = 0.02f;

	   public:
		float& Speed(){return speed;};
		void moveSec(double deltaSec);
		void HitFlip(float HitAngle);
		void drawn();
		friend void breakout::colide(pbola &);
	} bola;
	
	class pbar {
		float x, y, speed = 1.25;

	   public:
		void move(double deltaSec, int direction);
		void drawn();
		pbar();
	} bar;
	class tile {
		float x, y;
		bool alive = 1;
		vec4f color = {0.0, 0.0, 1.0, 0.0};

	   public:
		void hit();
		void drawn();
		tile(int indexX, int indexY);
	};
	std::vector<std::vector<tile>> tilemap;
};