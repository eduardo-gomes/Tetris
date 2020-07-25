#include <DuEngine/DuEngine.hpp>
//constants
constexpr int RIGHT = 1, LEFT = -1;

class breakout : public scene::BaseScene {
   public:
	breakout();
	~breakout();
	void Update(int64_t);
	void Render();
	void RenderGUI();

   private:
	class pbola;
	void colide(pbola &);
	class pbola {
		float x, y, angle, speed, diameter = 0.02f;

	   public:
		void moveSec(double deltaSec);
		void HitFlip(float HitAngle);
		inline void drawn(){
			vec3f pos = {x, y, 0.0f};
			constexpr vec4f color = {1.0, 1.0, 1.0, 1.0};
			vec2f size = {diameter, diameter};
			renderer->DrawnQuad(pos, color, size);
		}
		friend void breakout::colide(pbola &);
	} bola;
	class pbar {
		float x, y, xsize = 0.15, ysize = 0.02;

	   public:
		void move(double deltaSec, int direction);
		inline void drawn(){
			vec3f pos = {x, y, 0.0f};
			constexpr vec4f color = {0.0, 1.0, 1.0, 1.0};
			vec2f size = {xsize, ysize};
			renderer->DrawnQuad(pos, color, size);
		}
	} bar;
};
void breakout::colide(pbola &bola) {
	//detect colision
	float angle;//calc angle
	bola.HitFlip(angle);
	//do something with who soffered colision
	return;
}

void breakout::Update(int64_t deltaNano) {
	double deltaSec = deltaNano / 1e9;
	if (keyboard::a) bar.move(deltaSec, LEFT);
	if (keyboard::d) bar.move(deltaSec, RIGHT);
	bola.moveSec(deltaSec);
	colide(bola);
}

void breakout::Render() {
	//Drawn enemies
	bar.drawn();
	bola.drawn();
}

int main(){
	SetSetup([]() {});
	return Start("breakout");
}