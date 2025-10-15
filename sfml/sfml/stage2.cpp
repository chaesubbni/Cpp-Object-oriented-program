#include <iostream> 
#include <chrono>
#include <SFML/Graphics.hpp> 
#include <array>
#include <stack>
#include <ctime>
#include <fstream>
using namespace std;
// namespace��  �̿��� ���, ����  ���� 
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
			Invalid,
			Path,
			Visited,
			Wall,
			Finish,
			Start
		};
	}
	// template �Լ� 
	//
	// i_value is plus : true - false = 0 
	// i_value is minus: false - true = -1 
	template<typename value_type>
	char sign(const value_type i_value) {
		return (0 < i_value) - (0 > i_value);
	}

	// 2���� ���� ��  ��  ���¿� ���� �迭
 // (x, y) ���·� ��Ÿ���� ���ؼ� columns��  ���� ���� 
	template<typename value_type = MAP::Cell>
	using Map = std::array<std::array<value_type, MAP::ROWS>, MAP::COLUMNS>;
	// 2���� ��ǥ Ÿ��
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


// ���� ǥ��.
auto add = [](int x) { return x + 1; };

class File {
protected:
	string file_name;
	int count;
public:

	File(string _name) {
		file_name = _name;
		count = 0;
	}

	virtual void addcount(void) = 0; // �����Լ� ���.

};


class IFile : public File {
private:
	ofstream os;
public:
	IFile(string _name) : File(_name), os(_name) {}

	void addcount() {
		count = add(count); // ���� ���.
		os << "move count: " << count << endl;
		if (!os) {
			cout << "Fail" << endl; // ���� ���� ���� Ȯ��
		}
	}

	void Count() { // ��ü ���.
		ifstream is(file_name);
		string line;
		while (getline(is, line)) {
			cout << line << endl;
		}
	}

	void Count(int line_number) { // Ư�� �ٸ� ���.
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

/*
ȭ�鿡 ���� �׸��� �Լ�
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

			case gbl::MAP::Cell::Path:
			{
				_map_sprite.setColor(sf::Color(7, 232, 117));
				break;
			}

			case gbl::MAP::Cell::Visited:
			{
				_map_sprite.setColor(sf::Color(0, 36, 255));
				break;
			}

			case gbl::MAP::Cell::Wall:
			{
				_map_sprite.setColor(sf::Color(255, 60, 237));
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

gbl::MAP::Cell get_cell(const gbl::Position<short>& _cell, const gbl::Map<>& _map)
{
	if (0 <= _cell.first && 0 <= _cell.second && _cell.first < gbl::MAP::COLUMNS && _cell.second <
		gbl::MAP::ROWS)
	{
		return _map[_cell.first][_cell.second];
	}
	return gbl::MAP::Cell::Invalid;//invalid == Ÿ������ ����.
}
/*
_cell��  ���Ͽ�  ����  cell����  ��ǥ��  ��ȯ
*/
std::vector<gbl::Position<>> get_adjacent_cells(const gbl::Position<>& _cell, const gbl::Map<>& _map)
{
	std::vector<gbl::Position<>> adjacent_cells;
	std::vector<gbl::Position<>> diagonal_adjacent_cells;
	for (auto a = -1; a < 2; a++)
	{
		for (auto b = -1; b < 2; b++)
		{
			// �ڱ��ڽ�  ����
			if (0 == a && 0 == b) {
				continue;
			}
			gbl::MAP::Cell _type = get_cell(gbl::Position<short>(_cell.first + a, _cell.second + b), _map);
			// ���̰ų�, ����  ����  �ƴ�  ���  ����
			if (gbl::MAP::Cell::Invalid != _type && gbl::MAP::Cell::Wall != _type)
			{
				adjacent_cells.push_back(gbl::Position<>(a + _cell.first, b + _cell.second));
			}
		}
	}
	return adjacent_cells;
}

std::vector<gbl::Position<>> get_adjacent_cells_include_wall(gbl::Position<>& _cell, gbl::Map<>& _map)
{
	std::vector<gbl::Position<>> adjacent_cells;
	std::vector<gbl::Position<>> diagonal_adjacent_cells;
	for (auto a = -1; a < 2; a++)
	{
		for (auto b = -1; b < 2; b++)
		{
			// �ڱ��ڽ�  ����
			if (0 == a && 0 == b) {
				continue;
			}
			gbl::MAP::Cell _type = get_cell(gbl::Position<short>(_cell.first + a, _cell.second + b), _map);
			// ���̰ų�, ����  ����  �ƴ�  ���  ����
			if (gbl::MAP::Cell::Invalid != _type)
			{
				adjacent_cells.push_back(gbl::Position<>(a + _cell.first, b + _cell.second));
			}
		}
	}
	return adjacent_cells;
}




/*
heuristic score ���
*/
float calculate_h_score(const gbl::Position<>& _cell_0, const gbl::Position<>& _cell_1)
{
	float distance_x = abs(_cell_1.first - _cell_0.first);
	float distance_y = abs(_cell_1.second - _cell_0.second);
	return std::max(distance_x, distance_y) + std::min(distance_x, distance_y) * (sqrt(2) - 1);
}

bool astar_search(unsigned short& _path_length,
	unsigned short& _total_checks,
	std::chrono::microseconds& _duration,
	std::map<gbl::Position<>, gbl::Position<>>& _previous_cell,//���� �� ����.
	std::vector<gbl::Position<>>& _path_vector,// ��θ� ã�� �������� ����� ����
	gbl::Map<float>& _f_scores,
	gbl::Map<float>& _g_scores,
	const gbl::Map<float>& _h_scores,
	const gbl::Position<>& _finish_position,
	const gbl::Position<>& _start_position,
	gbl::Map<>& _map,
	stack<gbl::Position<>>& st,
	int& flag)
{
	// �ð� ����
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	// 8���� A* �ݺ��� �����ϰ�, frame��  �ٽ� �׸���.
	for (unsigned short a = 0; a < gbl::PATHFINDING::CHECKS_PER_FRAME; a++)
	{
		// �˰��� ���� -> ��ΰ� ����.
		if (1 == _path_vector.empty())
		{
			_duration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);
			flag = 100;
			return true;
		}
		std::vector<gbl::Position<>>::iterator min_f_cell_iterator = _path_vector.begin();
		gbl::Position<> min_f_cell;
		// f score���� ����  ���� cell��  ã�Ƴ���.
		// ��  �κ��� priority queue ���� �̿��Ͽ� ������ �����ϴ�. 
		//1
		for (std::vector<gbl::Position<>>::iterator a = 1 + _path_vector.begin(); a != _path_vector.end(); a++)
		{
			if (_f_scores[a->first][a->second] < _f_scores[min_f_cell_iterator->first][min_f_cell_iterator->second])
			{
				min_f_cell_iterator = a;
			}
		}
		min_f_cell = *min_f_cell_iterator;

		_path_vector.erase(min_f_cell_iterator);
		_map[min_f_cell.first][min_f_cell.second] = gbl::MAP::Cell::Visited;
		_total_checks++;
		// ���� �������� ������ ��� 
		if (min_f_cell == _finish_position)
		{
			gbl::Position<> path_cell = min_f_cell;
			do
			{
				_path_length++;
				_map[path_cell.first][path_cell.second] = gbl::MAP::Cell::Path;
				st.push(path_cell);
				path_cell = _previous_cell.at(path_cell);
			} while (_start_position != path_cell);
			_duration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);
			return 1;
		}
		// �ֺ� ���鿡 ���ؼ� ������Ʈ
		for (const gbl::Position<>& adjacent_cell : get_adjacent_cells(min_f_cell, _map))
		{
			if (gbl::MAP::Cell::Visited != _map[adjacent_cell.first][adjacent_cell.second])
			{
				float g_score = _g_scores[min_f_cell.first][min_f_cell.second];
				if (abs(adjacent_cell.first - min_f_cell.first) == abs(adjacent_cell.second - min_f_cell.second))
				{  // �밢�� �Ÿ�
					g_score += sqrt(2);
				}
				else
				{  // �����¿� �Ÿ�
					g_score++;
				}
				// g score (����  �Ÿ�)�� ��  ª������ ��쿡 ���ؼ���,
				if (g_score < _g_scores[adjacent_cell.first][adjacent_cell.second])
				{
					_previous_cell[adjacent_cell] = min_f_cell;
					_f_scores[adjacent_cell.first][adjacent_cell.second] = g_score + _h_scores[adjacent_cell.first][adjacent_cell.second];
					_g_scores[adjacent_cell.first][adjacent_cell.second] = g_score;
					// �̹�  �߰�����
					if (_path_vector.end() == std::find(_path_vector.begin(), _path_vector.end(), adjacent_cell))
					{
						_path_vector.push_back(adjacent_cell);
					}
				}
			}
		}
	}
	_duration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);
	return 0;
}


/*
A* �˰��� �ʱ�ȭ
*/
void astar_reset(bool& _finished,
	unsigned short& _path_length,
	unsigned short& _total_checks,
	std::chrono::microseconds& _duration,
	std::map<gbl::Position<>, gbl::Position<>>& _previous_cell,
	std::vector<gbl::Position<>>& _path_vector,
	gbl::Map<float>& _f_scores,
	gbl::Map<float>& _g_scores,
	gbl::Map<float>& _h_scores,
	gbl::Position<>& _finish_position,
	gbl::Position<>& _start_position,
	gbl::Map<>& _map)
{
	_finished = 0;
	_path_length = 0;
	_total_checks = 0;
	_duration = std::chrono::microseconds(0);
	_previous_cell.clear();
	_path_vector.clear();
	_path_vector.push_back(_start_position);

	for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
	{
		for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
		{
			_h_scores[a][b] = calculate_h_score(gbl::Position<>(a, b),
				_finish_position);
			if (a == _start_position.first && b == _start_position.second)
			{
				_f_scores[a][b] = _h_scores[a][b];
				_g_scores[a][b] = 0;
			}
			else
			{
				_f_scores[a][b] = FLT_MAX;
				_g_scores[a][b] = FLT_MAX;
			}
		}
	}
}


class Block {
private:
	gbl::Map<>* Bmap;

	bool cangblock(int startX, int startY) {
		// ������ ���� �� �� �ִ��� ����
		for (int i = 0; i < 5; i++) {
			if ((*Bmap)[startX + i][startY] != gbl::MAP::Cell::Empty) {
				return false;
			}
		}

		if ((*Bmap)[startX + 4][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX + 4][startY + 2] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX + 4][startY + 3] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX + 4][startY + 4] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}



	bool cannblock(int startX, int startY) {
		// ������ ���� �� �� �ִ��� ����
		for (int i = 0; i < 5; i++) {
			if ((*Bmap)[startX + i][startY + 4] != gbl::MAP::Cell::Empty) {
				return false;
			}
		}

		if ((*Bmap)[startX][startY] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 2] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 3] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}

	bool canlblock(int startX, int startY) {
		// ������ ���� �� �� �ִ��� ����
		for (int i = 0; i < 5; i++) {
			if ((*Bmap)[startX + i][startY] != gbl::MAP::Cell::Empty ||
				(*Bmap)[startX + i][startY + 2] != gbl::MAP::Cell::Empty ||
				(*Bmap)[startX + i][startY + 4] != gbl::MAP::Cell::Empty) {
				return false;
			}
		}

		if ((*Bmap)[startX + 4][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 3] != gbl::MAP::Cell::Empty) {
			return false;
		}
		return true;
	}

	bool candblock(int startX, int startY) {
		// ������ ���� �� �� �ִ��� ����
		for (int i = 0; i < 5; i++) {
			if ((*Bmap)[startX + i][startY] != gbl::MAP::Cell::Empty ||
				(*Bmap)[startX + i][startY + 4] != gbl::MAP::Cell::Empty) {
				return false;
			}
		}

		if ((*Bmap)[startX][startY + 1] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 2] != gbl::MAP::Cell::Empty ||
			(*Bmap)[startX][startY + 3] != gbl::MAP::Cell::Empty) {
			return false;
		}

		return true;
	}


public:
	Block(gbl::Map<>* _map) {
		Bmap = _map;
	}

	void gblock() {
		int count = 0;
		while (1) {
			int startX = rand() % (gbl::MAP::COLUMNS - 4);
			int startY = rand() % (gbl::MAP::ROWS - 4);
			if (cangblock(startX, startY)) {
				for (int i = 0; i < 5; i++) {
					(*Bmap)[startX + i][startY] = gbl::MAP::Cell::Wall;
				}
				(*Bmap)[startX + 4][startY + 1] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX + 4][startY + 2] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX + 4][startY + 3] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX + 4][startY + 4] = gbl::MAP::Cell::Wall;
				count++;
			}
			if (count == 2) {
				break;
			}
		}
	}

	void nblock() {
		int count = 0;
		while (1) {
			int startX = rand() % (gbl::MAP::COLUMNS - 4);
			int startY = rand() % (gbl::MAP::ROWS - 4);
			if (cannblock(startX, startY)) {

				(*Bmap)[startX][startY] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 1] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 2] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 3] = gbl::MAP::Cell::Wall;

				for (int i = 0; i < 5; i++) {
					(*Bmap)[startX + i][startY + 4] = gbl::MAP::Cell::Wall;
				}
				count++;
			}
			if (count == 2) {
				break;
			}
		}
	}

	void lblock() {
		int count = 0;
		while (1) {
			int startX = rand() % (gbl::MAP::COLUMNS - 4);
			int startY = rand() % (gbl::MAP::ROWS - 4);
			if (canlblock(startX, startY)) {
				for (int i = 0; i < 5; i++) {
					(*Bmap)[startX + i][startY] = gbl::MAP::Cell::Wall;
					(*Bmap)[startX + i][startY + 2] = gbl::MAP::Cell::Wall;
					(*Bmap)[startX + i][startY + 4] = gbl::MAP::Cell::Wall;
				}

				(*Bmap)[startX + 4][startY + 1] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 3] = gbl::MAP::Cell::Wall;

				count++;
			}
			if (count == 2) {
				break;
			}
		}
	}

	void dblock() {
		int count = 0;
		while (1) {
			int startX = rand() % (gbl::MAP::COLUMNS - 4);
			int startY = rand() % (gbl::MAP::ROWS - 4);
			if (candblock(startX, startY)) {
				for (int i = 0; i < 5; i++) {
					(*Bmap)[startX + i][startY] = gbl::MAP::Cell::Wall;
					(*Bmap)[startX + i][startY + 4] = gbl::MAP::Cell::Wall;
				}
				(*Bmap)[startX][startY + 1] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 2] = gbl::MAP::Cell::Wall;
				(*Bmap)[startX][startY + 3] = gbl::MAP::Cell::Wall;

				count++;
			}
			if (count == 2) {
				break;
			}
		}
	}

};


class Create : public Block {
public:
	Create(gbl::Map<>* _map) :Block(_map) {

	}

	void create() {
		gblock();
		nblock();
		lblock();
		dblock();
		gblock();
		nblock();
		lblock();
		dblock();
	}
};




int main() {
	srand(time(NULL));
	bool astar_finished = false;
	unsigned short astar_path_length = 0;
	unsigned short astar_total_checks = 0;
	// Astar ����ð� ����
	// == chrono::duration<double, micro> 
	chrono::microseconds astar_duration(0);
	map<gbl::Position<>, gbl::Position<>> astar_previous_cell;
	vector<gbl::Position<>> astar_path_vector;
	gbl::Map<float> astar_h_scores = {};
	gbl::Map<float> astar_g_scores = {};
	gbl::Map<float> astar_f_scores = {};
	//--------- SFML ȭ��  �׸��� ---------------
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
	//sf::Texture font_texture;
	//font_texture.loadFromFile("Resources/Images/Font.png"); 
	sf::Texture map_texture;
	sf::Texture hamster_texture;
	map_texture.loadFromFile("Map.png");
	try {
		if (!hamster_texture.loadFromFile("hamster.png")) {
			// ���� �ε� ���п� ���� ó��
			string str = "fail";
			throw str;
		}
		// ���������� �ε�� ���� ����
	}
	catch (string str) {
		cout << str << endl;
	}
	gbl::Map<> map = {};
	gbl::Position<> finish_position(0, 0);
	gbl::Position<> start_position(0, 0);
	gbl::Position<short> mouse_cell_start;
	stack<gbl::Position<>> st;
	IFile myfile("Count.txt"); // ���� ����.
	int count_end = 0;
	int hamster_speed = 0;
	int flag = 0;

	// ��� ����  empty���·� ����
	for (array<gbl::MAP::Cell, gbl::MAP::ROWS>& column : map) {
		column.fill(gbl::MAP::Cell::Empty);
	}

	Create C(&map);
	C.create();


	map_sprite.setTexture(map_texture);
	hamster_sprite.setTexture(hamster_texture);
	previous_time = chrono::steady_clock::now();
	while (1 == window.isOpen()) {
		chrono::microseconds delta_time =
			chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time += delta_time;
		// �ּ� frame �����ð� == FRAME_DURATION 
		while (gbl::SCREEN::FRAME_DURATION <= lag)
		{
			// ȭ���� �ٽ� �׸� �ʿ䰡 �ִ��� �˻� 
			bool map_updated = 0;
			// ���� ��  ����ð� ����
			lag -= gbl::SCREEN::FRAME_DURATION;

			// Ű���� �Է¿� ����  �˻�
			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}

				case sf::Event::MouseButtonPressed:
					// drag and drop��  ���� ��  ����  ����
			// ���콺 ��ư�� ����  ���¿��� ������ ��  ��  ���� ����

					if (!finish_position_set) {
						if (event.mouseButton.button == sf::Mouse::Left) {

							// ���ο� ���� ��ġ ��������

							gbl::Position<short> new_finish = gbl::get_mouse_cell(window);

							// �� ��ġ�� ��ȿ�ϰ� ���� ��ġ�� �ƴ��� Ȯ��
							if (new_finish != start_position &&
								new_finish.first < gbl::MAP::COLUMNS &&
								new_finish.second < gbl::MAP::ROWS &&
								map[new_finish.first][new_finish.second] == gbl::MAP::Cell::Empty) {

								// ���� ��ġ �����ϰ� �÷��׸� true�� ����
								finish_position = new_finish;
								finish_position_set = true;

								// A* �˻��� �����ϰ�, �� �̻� �������� ����
								pause_search = false;
								map_updated = true;
								astar_finished = false;

								// class IFile�� count�� ����.
								myfile.addcount();
								myfile.Count(count_end);
								count_end = add(count_end); // ���� ǥ������ ������ �Լ��� �̿��ؼ� count_end�� 1 ����.

							}

						}
					}
					break;
				}
			}

			if (map_updated)
			{
				astar_reset(astar_finished,
					astar_path_length,
					astar_total_checks,
					astar_duration,
					astar_previous_cell,
					astar_path_vector,
					astar_f_scores,
					astar_g_scores,
					astar_h_scores,
					finish_position,
					start_position,
					map);
				map_updated = false;
			}


			if (!pause_search && finish_position_set)
			{
				if (false == astar_finished)
				{
					astar_finished = astar_search(
						astar_path_length,
						astar_total_checks,
						astar_duration,
						astar_previous_cell,
						astar_path_vector,
						astar_f_scores,
						astar_g_scores,
						astar_h_scores,
						finish_position,
						start_position,
						map,
						st,
						flag);
				}
			}

			if (flag == 100) {
				gbl::Position<> k = finish_position;
				vector<gbl::Position<>> store = get_adjacent_cells_include_wall(k, map);

				gbl::Position<> l = store[0];

				float sub = abs(start_position.first - l.first) + abs(start_position.second - l.second);

				while (1) {
					float sub2;
					for (gbl::Position<>& i : store) {
						sub2 = abs(start_position.first - i.first) + abs(start_position.second - i.second);
						if (sub > sub2) {
							sub = sub2;
							l = i;
						}
					}
					if (map[l.first][l.second] == gbl::MAP::Cell::Visited || map[l.first][l.second] == gbl::MAP::Cell::Start) {
						map[finish_position.first][finish_position.second] = gbl::MAP::Cell::Empty;
						finish_position = l;
						break;
					}
					store = get_adjacent_cells_include_wall(l, map);
				}
				store.clear();

				for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
				{
					for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
					{
						if (map[a][b] != gbl::MAP::Cell::Wall) {
							map[a][b] = gbl::MAP::Cell::Empty;
						}
					}
				}

				astar_reset(astar_finished,
					astar_path_length,
					astar_total_checks,
					astar_duration,
					astar_previous_cell,
					astar_path_vector,
					astar_f_scores,
					astar_g_scores,
					astar_h_scores,
					finish_position,
					start_position,
					map);

				finish_position_set = true;
				flag = 0;
				pause_search = false;
				astar_finished = false;
			}

			hamster_speed++;
			if (hamster_speed == 3) {
				if (!st.empty()) {
					map[start_position.first][start_position.second] = gbl::MAP::Cell::Path;
					start_position.first = st.top().first;
					start_position.second = st.top().second;
					st.pop();
				}

				else if (start_position == finish_position) {
					finish_position_set = false;
					pause_search = true;

					for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
					{
						for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
						{
							if (map[a][b] != gbl::MAP::Cell::Wall) {
								map[a][b] = gbl::MAP::Cell::Empty;
							}
						}
					}
				}
				hamster_speed = 0;
			}

			// ȭ�� �ٽ�  �׸��� 
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
	}
	return 0;
}
