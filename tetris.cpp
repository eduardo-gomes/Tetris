#include <chrono>
#include <future>

#include <DuEngine/DuEngine.hpp>
std::shared_ptr<audio::WAVE> putBlock;
bool assets_loaded = false;
namespace scene {
class Tetris : public BaseScene {
   private:
	unsigned int points;
	class TetrisMap;
	TetrisMap* Map;
	class peca;
	peca *atual, *next;
	class pecaT;
	class pecaCubo;
	class bagulhinPraEsquerda;
	class bagulhinPraDireita;
	class pecaI;
	peca* randomPeca();
	friend peca;
	Texture *blockText;

   public:
	Tetris();
	~Tetris();

	void Update(int64_t delta);
	void Render();
	void RenderGUI();
};

constexpr unsigned int MAP_HEIGHT = 16, MAP_WIDTH = 9;
class Tetris::TetrisMap{
	Tetris* game;
	bool ocuped[MAP_HEIGHT][MAP_WIDTH];
	vec4f color[MAP_HEIGHT][MAP_WIDTH];
	public:
	inline bool isOcuped(unsigned int x, unsigned int y){return ocuped[y][x];}
	inline void Ocupe(unsigned int x, unsigned int y, const vec4f &cor){
		ocuped[y][x] = 1;
		color[y][x] = cor;
	}
	void render(){
		for (unsigned int x = 0; x < MAP_WIDTH; ++x)
			for (unsigned int y = 0; y < MAP_HEIGHT; ++y)
				if (ocuped[y][x]) renderer->DrawnQuad({(float)x, (float)y, 0.0f}, color[y][x], {1.0f, 1.0f}, *game->blockText);
	}
	TetrisMap(Tetris* g): game(g){
		for(unsigned int y = 0; y < MAP_HEIGHT; ++y)
			memset(ocuped[y], 0, sizeof(ocuped[0]));
	}
	void update(){
		for (unsigned int y = 0; y < MAP_HEIGHT; ++y){
			bool isFull = 1;
			for(unsigned int x = 0; x < MAP_WIDTH; ++x)
				isFull = isFull && ocuped[y][x];
			if(isFull){
				for (unsigned int y2 = y+1; y2 < MAP_HEIGHT; ++y2) {
					memcpy(&ocuped[y2 - 1], &ocuped[y2], sizeof(ocuped[y2]));
					memcpy(&color[y2 - 1], &color[y2], sizeof(color[y2]));
				}
				game->points += MAP_WIDTH * 10;
				if(y > 0) y--;
			}
		}
	}
};

class Tetris::peca {
	Tetris* game;
   protected:
	const vec4f cor;
	const float (&positions)[4][4][2];
	int rotation = 0;
	int xpos, ypos;
	peca(const float (&arr)[4][4][2], Tetris* t) : game(t), cor({Random::getf(), Random::getf(), Random::getf(), 1.0f}), positions(arr), xpos(MAP_WIDTH/2-1), ypos(MAP_HEIGHT-3){}

   public:
	virtual ~peca(){}
	inline bool rotate(bool clockwise) {
		int oldRotation = rotation;
		rotation = (rotation + (clockwise ? 1 : 3)) % 4;
		if (!isInsideMap(xpos, ypos) || colide(xpos, ypos)) {
			rotation = oldRotation;
			return 0;
		}
		return 1;
	}
	inline bool colide(int x, int y){
		if(isInsideMap(x, y)){
			for(int i = 0; i < 4; ++i){
				unsigned int quady = positions[rotation % 4][i][1] + y, quadx = positions[rotation % 4][i][0] + x;
				if(game->Map->isOcuped(quadx, quady))
					return true;
			}
		}else return true;
		return false;
	}
	inline bool isInsideMap(int x, int y){return isInsideMapH(x) && isInsideMapV(y);}
	inline bool isInsideMapV(int y) {
		if ((positions[rotation % 4][0][1] + y) >= MAP_HEIGHT || (positions[rotation % 4][0][1] + y) < 0 ||
			(positions[rotation % 4][1][1] + y) >= MAP_HEIGHT || (positions[rotation % 4][1][1] + y) < 0 ||
			(positions[rotation % 4][2][1] + y) >= MAP_HEIGHT || (positions[rotation % 4][2][1] + y) < 0 ||
			(positions[rotation % 4][3][1] + y) >= MAP_HEIGHT || (positions[rotation % 4][3][1] + y) < 0)
			return false;
		return true;
	}
	inline bool isInsideMapH(int x) {
		if ((positions[rotation % 4][0][0] + x) >= MAP_WIDTH || (positions[rotation % 4][0][0] + x) < 0 ||
			(positions[rotation % 4][1][0] + x) >= MAP_WIDTH || (positions[rotation % 4][1][0] + x) < 0 ||
			(positions[rotation % 4][2][0] + x) >= MAP_WIDTH || (positions[rotation % 4][2][0] + x) < 0 ||
			(positions[rotation % 4][3][0] + x) >= MAP_WIDTH || (positions[rotation % 4][3][0] + x) < 0)
			return false;
		return true;
	}
	int move(int x, int y) {
		if (!isInsideMapH(xpos + x)) return -1;
		if (colide(xpos + x, ypos + y)) return 0;
		xpos += x;
		ypos += y;
		return 1;
	}
	inline bool cantMove(int x, int y) {
		return colide(xpos + x, ypos + y);
	}
	inline void die(){
		for (int i = 0; i < 4; ++i) {
			game->Map->Ocupe(positions[rotation % 4][i][0] + xpos, positions[rotation % 4][i][1] + ypos, cor);
		}
	}
	void render() {
		for (int i = 0; i < 4; ++i)
			renderer->DrawnQuad({positions[rotation % 4][i][0] + xpos, positions[rotation % 4][i][1] + ypos, 0.0f}, cor, {1.0f, 1.0f}, *game->blockText);
	}
	void disp(){
		constexpr float ydisp = MAP_HEIGHT - 4, xdisp = MAP_HEIGHT * (16.0/9*0.9) - 4/*peca size*/ - 5 /*dist from border*/;
		for (int i = 0; i < 4; ++i)
			renderer->DrawnQuad({positions[0][i][0] + xdisp, positions[0][i][1] + ydisp, 0.0f}, cor, {1.0f, 1.0f}, *game->blockText);
	}
};

class Tetris::pecaT : public Tetris::peca {
   public:
	const float positions[4][4][2] = {
		//[rotation] [piece] [cord]
		{{0.0f, 0.0f}, {1.0f, 0.0f}, {2.0f, 0.0f}, {1.0f, 1.0f}},	 //up
		{{0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 2.0f}, {1.0f, 1.0f}},	 //right
		{{0.0f, 2.0f}, {1.0f, 2.0f}, {2.0f, 2.0f}, {1.0f, 1.0f}},	 //down
		{{2.0f, 0.0f}, {2.0f, 1.0f}, {2.0f, 2.0f}, {1.0f, 1.0f}},	 //left
	};
	pecaT(Tetris*t) : peca(positions, t) {
	}
};
class Tetris::pecaCubo : public Tetris::peca {
   public:
	const float positions[4][4][2] = {
		//[rotation] [piece] [cord]
		{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},  //symetric
		{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},  //symetric
		{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},  //symetric
		{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},  //symetric
	};
	pecaCubo(Tetris* t) : peca(positions, t) {
	}
};
class Tetris::pecaI : public Tetris::peca {
   public:
	const float positions[4][4][2] = {
		//[rotation] [piece] [cord]
		{{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 2.0f}, {1.0f, -1.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {2.0f, 1.0f}, {3.0f, 1.0f}},  //V
		{{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 2.0f}, {1.0f, -1.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {2.0f, 1.0f}, {3.0f, 1.0f}},  //V
	};
	pecaI(Tetris* t) : peca(positions, t) {
	}
};
class Tetris::bagulhinPraEsquerda : public Tetris::peca {
   public:
	const float positions[4][4][2] = {
		//[rotation] [piece] [cord]
		{{1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 2.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 2.0f}, {2.0f, 2.0f}},  //V
		{{1.0f, 2.0f}, {1.0f, 1.0f}, {2.0f, 1.0f}, {2.0f, 0.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 2.0f}, {2.0f, 2.0f}},  //V
	};
	bagulhinPraEsquerda(Tetris* t) : peca(positions, t) {
	}
};
class Tetris::bagulhinPraDireita : public Tetris::peca {
   public:
	const float positions[4][4][2] = {
		//[rotation] [piece] [cord]
		{{1.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 1.0f}, {2.0f, 2.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {2.0f, 0.0f}},  //V
		{{1.0f, 2.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}},  //H
		{{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {2.0f, 0.0f}},  //V
	};
	bagulhinPraDireita(Tetris* t) : peca(positions, t) {
	}
};
Tetris::peca* Tetris::randomPeca(){
	unsigned int pecaNum = Random::get()%5;
	switch (pecaNum)	{
	case 0:
		return new pecaT(this);
	case 1:
		return new pecaCubo(this);
	case 2:
		return new pecaI(this);
	case 3:
		return new bagulhinPraEsquerda(this);
	case 4:
		return new bagulhinPraDireita(this);
	}
	throw std::domain_error("Invalid peça index");
}

void Tetris::Update(int64_t delta) {
	constexpr int64_t moveTime = 100*10e6, inputTime = 30*10e6;
	static int64_t time = 0, nextMoveTime = 0, nextInputTime = 0;
	time += delta;
	if(time < 0) throw std::overflow_error("why keep playing for so long");
	if (time >= nextMoveTime) {
		nextMoveTime = time + moveTime;
		atual->move(0, -1);
	}
	if (time >= nextInputTime) {}
		nextInputTime = time + inputTime;
		if (keyboard::w) if (atual->rotate(1))  	keyboard::w = 0;
		if (keyboard::d) if (atual->move(1, 0)) 	keyboard::d = 0;
		if (keyboard::a) if (atual->move(-1, 0)) 	keyboard::a = 0;
		if (keyboard::space){
			while (!atual->cantMove(0, -1)) atual->move(0, -1);
		keyboard::space = 0;
		}
	//}
	if (atual->cantMove(0, -1)) {
		//to do when die: check is sound is loaded
		audio::audioOut->EnqueueSound(putBlock);
		atual->die();
		delete atual;
		atual = next;
		next = randomPeca();
		if(atual->cantMove(0, 0)){
			printf("Can't spawn, points: %d \n", points);
			new Tetris;
		}
	}
}

void Tetris::Render() {
	for (unsigned int h = 0; h < MAP_HEIGHT; ++h)
		for (unsigned int w = 0; w < MAP_WIDTH; ++w) {
			renderer->DrawnQuad({(float)w, (float)h, 0.0f}, {0.0f, 0.35f, 1.0f, 0.10f}, {0.95f, 0.95f});
		}
	Map->update();
	Map->render();
	atual->render();
	next->disp();
	renderer->Drawn();
}
void Tetris::RenderGUI() {
	ImGui::Begin("Tetris");
	ImGui::Text("Points: %u", this->points);
	static bool Info = 1;
	ImGui::Checkbox("Info", &Info);
	/*if (ImGui::Button("Next"))
		new renderertestrotate;*/
	ImGui::End();
	if (Info) renderer->DispInfo();
}

Tetris::Tetris() {
	Map = new TetrisMap(this);
	float x = (float)MAP_WIDTH / 2, y = (float)MAP_HEIGHT / 2, z = y/tan(screen::fovy/2*M_PI/180)*1.1;
	Renderer::LookAt(x, y, z, x, y, 0.0f, 0.0f, 1.0f, 0.0f);
	atual = new pecaT(this);
	next = randomPeca();
	blockText = new Texture("assets/bloco.bmp");
}
Tetris::~Tetris() {
	delete Map;
	delete atual;
	delete next;
}

}  // namespace scene
void SetupFunc();
int main(){
	Setup = SetupFunc;
	Start("DuTetris", 1);
	return 0;
}

void load_assets(){
	auto &korobeiniki = Man::Manager::Insatance->LoadOGG("assets/korobeiniki.ogg");
	putBlock = Man::Manager::Insatance->LoadOGG("assets/sfx_sounds_impact1.ogg");
	audio::audioOut->EnqueueMusic(korobeiniki);//Need to make controlable outside this function
	assets_loaded = true;
}
void SetupFunc() {
	std::thread(load_assets).detach();
	new scene::Tetris;
}
#ifdef _WIN32
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;
	return main();
}
#endif
