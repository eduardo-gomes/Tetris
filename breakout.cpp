#include "breakout.hpp"
inline void breakout::tile::drawn() {
	renderer->DrawnQuad({x, y, 0.0}, color, {TILES_H_SIZE, TILES_V_SIZE});
}
breakout::tile::tile(int indexX, int indexY) {
	x = TILES_BORDER_ABSOLUTE * indexX + TILES_H_SIZE * (indexX - 1) + TILES_H_SIZE / 2;
	y = TILES_BORDER_ABSOLUTE * indexY + TILES_V_SIZE * (indexY - 1) + TILES_V_SIZE / 2 + TILES_V_OFSSET;
}

breakout::pbar::pbar() {
	x = 0;
	y = BAR_Y_POS;
}

inline void breakout::pbar::drawn() {
	vec3f pos = {x, y, 0.0f};
	constexpr vec4f color = {0.0, 1.0, 1.0, 1.0};
	constexpr vec2f size = {BAR_H_SIZE, BAR_V_SIZE};
	renderer->DrawnQuad(pos, color, size);
}

inline void breakout::pbola::drawn() {
	vec3f pos = {x, y, 0.0f};
	constexpr vec4f color = {1.0, 1.0, 1.0, 1.0};
	vec2f size = {diameter, diameter};
	renderer->DrawnQuad(pos, color, size);
}

breakout::breakout() {
	tilemap.resize(TILES_ROWS);
	for (unsigned int row = 0; row < TILES_ROWS; ++row){
		tilemap[row].reserve(TILES_COLUMNS);
		for (unsigned int column = 0; column < TILES_COLUMNS; ++column) {
			tilemap[row].emplace_back(column, row);
		}
	}
}
breakout::~breakout(){
	tilemap.clear();
}

void breakout::colide(pbola &bola) {
	//detect colision
	//float angle;//calc angle
	//bola.HitFlip(angle);
	constexpr float margin = 0.98;
	if (bola.x > ASPECT_RATIO){
		bola.HitFlip(180);
		bola.x = ASPECT_RATIO * margin;
	}
	else if (bola.x < -ASPECT_RATIO){
		bola.HitFlip(0);
		bola.x = -ASPECT_RATIO * margin;
	}
	if (bola.y > 1){
		bola.HitFlip(270);
		bola.y = 1 * margin;
	}
	else if (bola.y < -1){
		bola.HitFlip(90);
		bola.y = -1 * margin;
	}
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
	renderer->DrawnQuad({0.0, 0.0, 0.0}, {0.0, 1.0, 1.0, 0.5}, {2.0 * ASPECT_RATIO, 2.0});
	//Drawn enemies
	bar.drawn();
	bola.drawn();
	renderer->Drawn();
}

void breakout::RenderGUI(){
	ImGui::Begin("Bola");
	ImGui::DragFloat("Angle", &bola.angle, 1.0, 0.0, 360.0);
	ImGui::End();
}

void breakout::pbola::moveSec(double deltaSec){
	x += cos(angle * M_PI / 180) * speed * deltaSec;
	y += sin(angle * M_PI / 180) * speed * deltaSec;
}
void breakout::pbar::move(double deltaSec, int direction){
	x += speed * deltaSec * direction;
}
inline float calcAngle(float normal, float incidente){
	float anguloDeIncidencia = incidente - normal;
	float anguloDeReflexao = 180 - anguloDeIncidencia;
	return anguloDeReflexao + normal;
}
void breakout::pbola::HitFlip(float HitAngle){
	//float angleDif = HitAngle - angle;
	angle = fmod(calcAngle(HitAngle, angle), 360.0f);
}

int main(){
	SetSetup([]() {new breakout;});
	return Start("breakout");
}