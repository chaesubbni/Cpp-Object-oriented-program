#include <iostream> 
#include <chrono>
#include <SFML/Graphics.hpp> 
#include <array>
#include <stack>
#include <ctime>
#include <fstream>
using namespace std;
// namespace를  이용한 상수, 변수  정의 
namespace gbl {
	namespace PATHFINDING {
		constexpr unsigned short CHECKS_PER_FRAME = 8;
	}
	namespace SCREEN {
		constexpr unsigned char RESIZE = 1;
		constexpr unsigned short HEIGHT = 700;
		constexpr unsigned short WIDTH = 1280;
		constexpr chrono::microseconds FRAME_DURATION(16667);
	}
	namespace MAP {
		constexpr unsigned char CELL_SIZE = 16;
		constexpr unsigned char ORIGIN_X = 8;
		constexpr unsigned char ORIGIN_Y = 8;
		constexpr unsigned short COLUMNS = 78;
		constexpr unsigned short ROWS = 43;
		enum Cell {
			Empty,
			GWall,
			NWall,
			DWall,
			LWall,
			BarWall,
			Finish,
			Start
		};
	}


	// 2차원 지도 각  셀  상태에 대한 배열
 // (x, y) 형태로 나타내가 위해서 columns를  먼저 지정 
	template<typename value_type = MAP::Cell>
	using Map = std::array<std::array<value_type, MAP::ROWS>, MAP::COLUMNS>;
	// 2차원 좌표 타입
	template<typename value_type = unsigned short>
	using Position = std::pair<value_type, value_type>;
	Position<short> get_mouse_cell(const sf::RenderWindow& i_window) {
		float mouse_x = sf::Mouse::getPosition(i_window).x - MAP::ORIGIN_X * SCREEN::RESIZE;
		float mouse_y = sf::Mouse::getPosition(i_window).y - MAP::ORIGIN_Y * SCREEN::RESIZE;
		return Position<short>(
			floor(mouse_x / MAP::CELL_SIZE / SCREEN::RESIZE),
			floor(mouse_y / MAP::CELL_SIZE / SCREEN::RESIZE));
	}
}

// 람다 표현.
auto add = [](int& x, int y) { x = x + y;  return; };


class File {
protected:
	string file_name;

public:
	int count;
	int cell70;
	File(string _name) {
		file_name = _name;
		count = 0;
		cell70 = 43 * 78 * 0.7;
	}

	virtual void addcount(int x) = 0; // 가상함수 사용.

};


class IFile : public File {
private:
	ofstream os;
public:
	IFile(string _name) : File(_name), os(_name) {}

	void addcount(int x) {
		add(count, x); // 람다 사용.
		os <<  "현재 생성된 wall셀 개수 : " << count << endl;
		if (!os) {
			cout << "Fail" << endl; // 파일 쓰기 오류 확인
		}
	}

	void Count() { // 전체 출력.
		cout << "70% 셀 개수: " << cell70 << endl;
	}

	void Count(int line_number) { // 특정 줄만 출력.
		ifstream is(file_name);
		string line;
		int current_line = 0;
		while (getline(is, line)) {
			if (current_line == line_number) {
				cout << line << endl;
				break;
			}
			current_line++;
		}
	}


};

class Block : public IFile {
private:
	gbl::Map<>* Bmap;
public:

	int cnt;
	Block(gbl::Map<>* _map, string name) : IFile(name) {
		Bmap = _map;
		cnt = 0;
	}


	bool cangblock(int startX, int startY) {
		// ㄱ자형 벽이 들어갈 수 있는지 검증

		if ((*Bmap)[startX][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX + 1][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX + 1][startY + 1] != gbl::MAP::Cell::Empty) {
			return false;
		}
		return true;
	}



	bool cannblock(int startX, int startY) {
		// ㄴ자형 벽이 들어갈 수 있는지 검증
		if ((*Bmap)[startX][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX][startY + 1] != gbl::MAP::Cell::Empty || (*Bmap)[startX + 1][startY + 1] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}

	bool canlblock(int startX, int startY) {
		// ㄹ자형 벽이 들어갈 수 있는지 검증
		for (int i = 0; i < 3; i++) {
			if ((*Bmap)[startX + i][startY] != gbl::MAP::Cell::Empty ||
				(*Bmap)[startX + i][startY + 2] != gbl::MAP::Cell::Empty ||
				(*Bmap)[startX + i][startY + 4] != gbl::MAP::Cell::Empty) {
				return false;
			}
		}

		if ((*Bmap)[startX + 2][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 3] != gbl::MAP::Cell::Empty) {
			return false;
		}
		return true;
	}

	bool candblock(int startX, int startY) {
		// ㄷ자형 벽이 들어갈 수 있는지 검증
		if ((*Bmap)[startX][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX + 1][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 2] != gbl::MAP::Cell::Empty || (*Bmap)[startX + 1][startY + 2] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}

	bool canmminibarblock(int startX, int startY) {
		if ((*Bmap)[startX][startY] != gbl::MAP::Cell::Empty || (*Bmap)[startX][startY + 1] != gbl::MAP::Cell::Empty || (*Bmap)[startX][startY + 2] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}



	void gblock() {


		int startX = rand() % (gbl::MAP::COLUMNS - 1);
		int startY = rand() % (gbl::MAP::ROWS - 1);

		if (startX == 0 && startY == 0) {
			return;
		}

		if (cangblock(startX, startY)) {
			(*Bmap)[startX][startY] = gbl::MAP::Cell::GWall;
			(*Bmap)[startX + 1][startY] = gbl::MAP::Cell::GWall;
			(*Bmap)[startX + 1][startY + 1] = gbl::MAP::Cell::GWall;
			IFile::addcount(3);
			cnt++;
			return;
		}

	}

	void nblock() {


		int startX = rand() % (gbl::MAP::COLUMNS - 1);
		int startY = rand() % (gbl::MAP::ROWS - 1);
		if (startX == 0 && startY == 0) {
			return;
		}
		if (cannblock(startX, startY)) {
			(*Bmap)[startX][startY] = gbl::MAP::Cell::NWall;
			(*Bmap)[startX][startY + 1] = gbl::MAP::Cell::NWall;
			(*Bmap)[startX + 1][startY + 1] = gbl::MAP::Cell::NWall;
			IFile::addcount(3);
			cnt++;
			return;
		}
	}


	void lblock() {


		int startX = rand() % (gbl::MAP::COLUMNS - 2);
		int startY = rand() % (gbl::MAP::ROWS - 4);
		if (startX == 0 && startY == 0) {
			return;
		}
		if (canlblock(startX, startY)) {
			for (int i = 0; i < 3; i++) {
				(*Bmap)[startX + i][startY] = gbl::MAP::Cell::LWall;
				(*Bmap)[startX + i][startY + 2] = gbl::MAP::Cell::LWall;
				(*Bmap)[startX + i][startY + 4] = gbl::MAP::Cell::LWall;
			}

			(*Bmap)[startX + 2][startY + 1] = gbl::MAP::Cell::LWall;
			(*Bmap)[startX][startY + 3] = gbl::MAP::Cell::LWall;
			IFile::addcount(11);
			cnt++;
			return;
		}

	}

	void dblock() {


		int startX = rand() % (gbl::MAP::COLUMNS - 1);
		int startY = rand() % (gbl::MAP::ROWS - 2);
		if (startX == 0 && startY == 0) {
			return;
		}
		if (candblock(startX, startY)) {
			(*Bmap)[startX][startY] = gbl::MAP::Cell::DWall;
			(*Bmap)[startX + 1][startY] = gbl::MAP::Cell::DWall;
			(*Bmap)[startX][startY + 1] = gbl::MAP::Cell::DWall;
			(*Bmap)[startX][startY + 2] = gbl::MAP::Cell::DWall;
			(*Bmap)[startX + 1][startY + 2] = gbl::MAP::Cell::DWall;
			IFile::addcount(5);
			cnt++;
			return;
		}

	}


	void minibarblock() {

		int startX = rand() % (gbl::MAP::COLUMNS);
		int startY = rand() % (gbl::MAP::ROWS - 2);

		if (startX == 0 && startY == 0) {
			return;
		}

		if (canmminibarblock(startX, startY)) {
			(*Bmap)[startX][startY] = gbl::MAP::Cell::BarWall;
			(*Bmap)[startX][startY + 1] = gbl::MAP::Cell::BarWall;
			(*Bmap)[startX][startY + 2] = gbl::MAP::Cell::BarWall;
			IFile::addcount(3);
			cnt++;
			return;
		}

	}

	void create() {

		while (cell70 > IFile::count) {

			
			switch (rand() % 6) {  // 6개의 장애물 유형 중 하나를 무작위로 선택
			case 0: // gblock
				gblock();
				break;
			case 1: // nblock
				nblock();
				break;
			case 2: // lblock
				lblock();
				break;
			case 3: // dblock
				dblock();
				break;
			case 4: // minibarblock
				minibarblock();
				break;
			}

		}

		Count();
		Count(cnt - 1);
	}
};



/*
화면에 셀을 그리는 함수
*/
void draw_map(const unsigned short _x,
	const unsigned short _y,
	const gbl::Position<>& _finish_position,
	const gbl::Position<>& _start_position,
	sf::RenderWindow& _window,
	sf::Sprite& _map_sprite,
	sf::Sprite& _hamster_sprite,
	gbl::Map<>& _map)
{
	for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
	{
		for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
		{
			_map_sprite.setPosition((float)gbl::MAP::ORIGIN_X + _x + a * gbl::MAP::CELL_SIZE,
				(float)gbl::MAP::ORIGIN_Y + _y + b * gbl::MAP::CELL_SIZE);
			if (a == _start_position.first && b == _start_position.second)
			{
				_map[a][b] = gbl::MAP::Cell::Start;
				//_hamster_sprite.setPosition((float)gbl::MAP::ORIGIN_X + _x + a * gbl::MAP::CELL_SIZE,
					//(float)gbl::MAP::ORIGIN_Y + _y + b * gbl::MAP::CELL_SIZE);
			}


			else if (a == _finish_position.first && b == _finish_position.second)
			{
				_map[a][b] = gbl::MAP::Cell::Finish;
				//_map_sprite.setColor(sf::Color(255, 0, 0));
			}

			gbl::MAP::Cell cell_type = _map[a][b];
			switch (cell_type)
			{
			case gbl::MAP::Cell::Empty:
			{
				_map_sprite.setColor(sf::Color(36, 36, 85));
				break;
			}

			case gbl::MAP::Cell::GWall:
			{
				_map_sprite.setColor(sf::Color(237, 28, 36));
				break;
			}

			case gbl::MAP::Cell::NWall:
			{
				_map_sprite.setColor(sf::Color(237, 198, 20));
				break;
			}

			case gbl::MAP::Cell::DWall:
			{
				_map_sprite.setColor(sf::Color(46, 237, 58));
				break;
			}
			case gbl::MAP::Cell::LWall:
			{
				_map_sprite.setColor(sf::Color(36, 109, 237));
				break;
			}
			case gbl::MAP::Cell::BarWall:
			{
				_map_sprite.setColor(sf::Color(152, 12, 237));
				break;
			}
			case gbl::MAP::Cell::Finish:
			{
				_map_sprite.setColor(sf::Color(255, 0, 0));
				break;
			}
			case gbl::MAP::Cell::Start:
			{
				_hamster_sprite.setPosition((float)gbl::MAP::ORIGIN_X + _x + a * gbl::MAP::CELL_SIZE,
					(float)gbl::MAP::ORIGIN_Y + _y + b * gbl::MAP::CELL_SIZE);
				break;
			}
			}

			_window.draw(_map_sprite);
		}
	}
	_window.draw(_hamster_sprite);
}





int main() {
	srand(time(NULL));
	bool astar_finished = false;
	unsigned short astar_path_length = 0;
	unsigned short astar_total_checks = 0;
	// Astar 수행시간 저장
	// == chrono::duration<double, micro> 
	chrono::microseconds astar_duration(0);
	map<gbl::Position<>, gbl::Position<>> astar_previous_cell;
	vector<gbl::Position<>> astar_path_vector;
	gbl::Map<float> astar_h_scores = {};
	gbl::Map<float> astar_g_scores = {};
	gbl::Map<float> astar_f_scores = {};
	//--------- SFML 화면  그리기 ---------------
	bool mouse_pressed = 0;
	bool pause_search = true;
	bool finish_position_set = false;
	chrono::microseconds lag(0);
	chrono::steady_clock::time_point previous_time;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(gbl::SCREEN::RESIZE * gbl::SCREEN::WIDTH,
		gbl::SCREEN::RESIZE * gbl::SCREEN::HEIGHT), "Pathfinding by A*", sf::Style::Close);
	window.setView(sf::View(sf::FloatRect(0, 0, gbl::SCREEN::WIDTH, gbl::SCREEN::HEIGHT)));
	sf::Sprite map_sprite;
	sf::Sprite hamster_sprite;
	
	sf::Texture map_texture;
	sf::Texture hamster_texture;
	map_texture.loadFromFile("Map.png");
	try {
		if (!hamster_texture.loadFromFile("hamster.png")) {
			// 파일 로드 실패에 대한 처리
			string str = "fail";
			throw str;
		}
		// 성공적으로 로드된 후의 로직
	}
	catch (string str) {
		cout << str << endl;
	}
	map_sprite.setTexture(map_texture);
	hamster_sprite.setTexture(hamster_texture);
	
	gbl::Map<> map = {};
	gbl::Position<> finish_position(0, 0);
	gbl::Position<> start_position(0, 0);
	
	// 모든 셀을  empty상태로 설정
	for (array<gbl::MAP::Cell, gbl::MAP::ROWS>& column : map) {
		column.fill(gbl::MAP::Cell::Empty);
	}

	Block B(&map, "hamster.txt");
	B.create();

	while (1 == window.isOpen()) {
		while (1 == window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}
			}
		}
		
		// 화면 다시  그리기 
		window.clear();
		draw_map(0, 0,
			finish_position,
			start_position,
			window,
			map_sprite,
			hamster_sprite,
			map);
		window.display();
	}

	return 0;
}
