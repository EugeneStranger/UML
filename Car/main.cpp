#include <iostream>
#include <conio.h>
#include <thread>
#include <Windows.h>
//#include <conio.h>
using namespace std::chrono_literals;
using namespace std;
using std::cout;
using std::cin;
using std::endl;

#define Enter 13
#define Escape 27
#define delimiter "\n--------------------------------------------------------------\n"

#define MIN_TANK_VOLUME		20
#define MAX_TANK_VOLUME		120 

class Tank
{
	const int VOLUME;
	double fuel_level;
public:
	int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0) return;
		if (fuel_level + fuel < VOLUME)fuel_level += fuel;
		else fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	Tank(int volume)
		:VOLUME
		(
			volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
			volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
			volume
		)
	{
		this->fuel_level = 0;
		cout << "Tank is ready:\t\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over:\t\t" << this << endl;
	}
	void info()const
	{
		cout << "Volume:\t" << VOLUME << " liters\n";
		cout << "Fuel level:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION	3
#define MAX_ENGINE_CONSUMPTION	30

class Engine
{
	const double DEFAULT_CONSUPTION;
	double consumption;
	double consumption_per_second;
	bool is_started;
public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption_per_second(double consumption)
	{
		consumption_per_second = consumption * 3e-5;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	Engine(double consumption)
		:DEFAULT_CONSUPTION
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		)
	{
		this->consumption = DEFAULT_CONSUPTION;
		set_consumption_per_second(consumption);
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over:\t\t" << this << endl;
	}
	void info()const
	{
		cout << "Consumption:\t" << consumption << " liters per 100 km.\n";
		cout << "Consumption:\t" << consumption_per_second << " liters per 1 second.\n";
	}
};
#define MAX_SPEED_LOWER_LEVEL	30
#define MAX_SPEED_UPPER_LEVEL	400
class Car
{
	Engine engine;
	Tank tank;
	int speed;
	const int MAX_SPEED;
	bool driver_inside;
	struct Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
	}threads;
public:
	int get_speed()const
	{
		return speed;
	}
	int get_MAX_SPEED()const
	{
		return MAX_SPEED;
	}
	Car(double consumption, int volume, int max_speed) :engine(consumption), tank(volume),
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LEVEL ? MAX_SPEED_LOWER_LEVEL :
			max_speed > MAX_SPEED_UPPER_LEVEL ? MAX_SPEED_UPPER_LEVEL :
			max_speed
		)
	{
		driver_inside = false;
		cout << "You car is ready to go\t" << this << endl;
	}
	~Car()
	{
		cout << "Car is over:\t\t" << this << endl;
	}
	void get_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable()) threads.panel_thread.join();
		system("CLS");
		cout << "Outside" << endl;
	}
	void start()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			engine.start();
			threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
	}
	void control()
	{
		char key = 0;
		do
		{
			key = 0;
			if (_kbhit())  //‘ункци€ _kbhit() возвращает
				key = _getch();
			switch (key)
			{
			case Enter:
				if (driver_inside) get_out();
				else get_in();
				break;
			case 'F': case 'f':
			{
				if (driver_inside)
				{
					cout << "ƒл€ начала нужно выйти из машины" << endl;
					//std::this_thread::sleep_for(2s);
					break;
				}
				double fuel;
				cout << "¬ведите объЄм топлива: "; cin >> fuel;
				tank.fill(fuel);
				break;
			}
			case 'I': case 'i':
			{
				if (engine.started() && driver_inside)stop();
				else start();
				break;
			}
			case Escape:
			{
				stop();
				get_out();
			}
			}
			if (tank.get_fuel_level() == 0)stop();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");

			cout << "Fuel level:\t" << tank.get_fuel_level() << " liters\t";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0xCE);
				cout << "LOW FUEL!";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			std::this_thread::sleep_for(500ms);
		}
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK
#define CAR_CHECK
void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHECK
	Tank tank(150);
	tank.info();
	do
	{
		int fuel;
		cout << "¬ведите уровень топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (true);
#endif // TANK_CHECK
#ifdef ENGINE_CHECK
	Engine engine(10);
	cout << delimiter;
	engine.info();
	cout << delimiter;
#endif // ENGINE_CHECK
#ifdef CAR_CHECK

	Car bmw(10, 70, 250);
	bmw.control();
#endif // CAR_CHECK
}