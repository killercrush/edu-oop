#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>
#include <iomanip>
using namespace std;

struct Time {
	int h;
	int m;
	bool operator == (const Time& t){
		return (this->h == t.h && this->m == t.m);
	}
	bool operator > (Time & t){
		return t < *this;
	}
	bool operator <= (Time & t){
		return !(*this > t);
	}
	bool operator < (Time const & t){
		if (this->h != t.h)
			return (this->h < t.h);
		else
			return (this->m < t.m);
	}
	void addMinutes(int minutes){
		m += minutes;
		while (m > 59) {
			++h;
			m -= 60;
			if (h > 23)	h -= 24;
		}
	}
	void addHoures(int houres){
		h += houres;
		while (h > 23) h -= 24;
	}
	void print(){
		cout << this->toString() << endl;
	}
	string toString(){
		string h_str;
		string m_str;
		h_str = h < 10 ? "0" + to_string(h) : to_string(h);
		m_str = m < 10 ? "0" + to_string(m) : to_string(m);
		return  h_str + ":" + m_str;
	}
};

class Timer {
	Time time_;
public:
	Timer(){
		time_.h = 0;
		time_.m = 0;
	}
	Time getTime(){
		return time_;
	}
	void setTime(Time t){
		time_ = t;
	}
	void setTime(int h, int m){
		time_.h = h;
		time_.m = m;
	}
	void tick(){
		int step = 1;
		int delay_time = 10;
		time_.addMinutes(step);
		Sleep(delay_time);
		if (time_.m == 0) time_.print();
	}

};

struct ScheduleRow {
	Time time;
	string numBus;
	bool operator < (ScheduleRow const & t){
		return (this->time < t.time);
	}
};

class Schedule {
	vector<ScheduleRow> rows_;
public:
	Schedule(){

	}
	void addRow(ScheduleRow srow) {
		rows_.push_back(srow);
	}
	void generate(int const rec_count) {
		int step = 12 * 60 / rec_count;
		step = step % 59 + 1;
		Time t;
		t.h = 6;
		t.m = 0;
		for (int i = 0; i < rec_count; ++i){
			ScheduleRow srow;
			int inc = 1 + rand() % step;
			t.addMinutes(inc);
			srow.time = t;
			srow.numBus = to_string(i % 10 + 1);
			rows_.push_back(srow);
			t.addMinutes(step - inc);
		}
		this->sort();
	}
	void print(){
		cout << "Расписание:" << endl;
		cout << "Время" << "| " << "№ автобуса" << "|" << endl;
		vector<ScheduleRow>::iterator it = rows_.begin();
		while (it != rows_.end()){
			cout << setw(5) << it->time.toString() << "| "
				 << setw(10) << it->numBus << "|" << endl;
			++it;
		}
	}
	void sort(){
		std::sort(rows_.begin(), rows_.end());
	}
	string getBusNumByTime(Time t){
		vector<ScheduleRow>::iterator it = rows_.begin();
		while (it != rows_.end()){
			if (it->time == t) return it->numBus;
			++it;
		}
		return "";
	}
	ScheduleRow getCurrentScheduleRow(Time t){
		vector<ScheduleRow>::iterator it = rows_.begin();
		while (it != rows_.end()){
			if (it->time <= t && (it + 1)->time > t) return *it;
			++it;
		}
		ScheduleRow sr;
		Time tt;
		tt.h = 0;
		tt.m = 0;
		sr.time = tt;
		sr.numBus = "";
		return sr;
	}
};


class Passenger {
	string name_;
	string bus_num_;
	string bus_stop_name_;
public:
	Passenger(){
		name_ = "";
		bus_num_ = "";
	}
	Passenger(string name, string bus_num, string bus_stop_name){
		name_ = name;
		bus_num_ = bus_num;
		bus_stop_name_ = bus_stop_name;
	}
	string getName(){
		return name_;
	}
	string getBusNum(){
		return bus_num_;
	}
	string getBusStopName(){
		return bus_stop_name_;
	}
	void print(){
		cout << name_ << " ждет автобус № " << bus_num_ << " чтобы доехать до остановки " << bus_stop_name_ << endl;
	}
};

class BusStop;
class Bus {
	string num_;
	size_t capacity_;
	vector<Passenger*> passengers_;
public:
	Bus(string num, int capacity){
		num_ = num;
		capacity_ = capacity;
	}
	Bus(){
		num_ = "";
		capacity_ = 0;
	}
	string getNum(){
		return num_;
	}
	int getCapacity(){
		return capacity_;
	}
	int getPassCount(){
		return passengers_.size();
	}
	void print(){
		cout << "Автобус № " << num_ << " - Пассажиров: " << passengers_.size() << " - Вместимость: " << capacity_ << endl;
	}
	void printPassengersList(){
		vector<Passenger*>::iterator it = passengers_.begin();
		cout << "*** Список пассажиров ***" << endl;
		while (it != passengers_.end()){
			cout << (*it)->getName() << endl;
			++it;
		}
		cout << "*************************" << endl;
		cout << endl;
	}
	void Bus::gotoBusStop(BusStop&);
	bool addPassenger(Passenger* pass){
		if (passengers_.size() >= capacity_){
			cout << "В автобусе №" << num_ << " нет мест " << pass->getName() << " не смог сесть" << endl;
			return false;
		}
		passengers_.push_back(pass);
		cout << pass->getName() << " сел в автобус №" << num_ << endl;
	}
	void deboarding(string bus_stop_name){
		auto f = [bus_stop_name](Passenger* p){ return p->getBusNum() == bus_stop_name; };
		passengers_.erase(remove_if(passengers_.begin(), passengers_.end(), f), passengers_.end());
	}
};

class BusStop {
	string name_;
	vector<Passenger*> passengers_;
	Bus* curr_bus_;
public:
	BusStop(string name) {
		name_ = name;
		curr_bus_ = 0;
	}
	void setPassengers(vector<Passenger*> p){
		passengers_ = p;
	}
	Bus* getCurrentBus(){
		if (curr_bus_ == 0) cout << " на остановке нет автобуса" << endl;
		return curr_bus_;
	}
	string getName(){
		return name_;
	}
	int getPassCount(){
		return passengers_.size();
	}
	void printPassengersList(){
		vector<Passenger*>::iterator it = passengers_.begin();
		cout << "*** Список пассажиров на остановке***" << endl;
		while (it != passengers_.end()){
			cout << (*it)->getName() << endl;
			++it;
		}
		cout << "*************************************" << endl;
		cout << endl;
	}
	bool acceptBus(Bus* bus){
		if (curr_bus_ != 0){
			cout << "На остановке находится автобус № " << curr_bus_->getNum();
			return false;
		}
		cout << "Приехал автобус №" << bus->getNum() << endl;
		curr_bus_ = bus;
		bus->deboarding(name_);
		boarding();
		removeBus();
	}
	void boarding(){
		cout << "Посадка" << endl;
		vector<Passenger*>::iterator it = passengers_.begin();
		while (it != passengers_.end()){
			if ((*it)->getBusNum() == curr_bus_->getNum())
				if (curr_bus_->addPassenger(*it)){
					it = passengers_.erase(it);
					continue;
				}
			++it;
		}
		cout << endl;
	}
	void removeBus(){
		curr_bus_ = 0;
	}
};

void Bus::gotoBusStop(BusStop& bs){
	bs.acceptBus(this);
}
Passenger* get_pass_by_bus_num(vector<Passenger*> & pass_list, string const & bus_num){
	vector<Passenger*>::iterator it = pass_list.begin();
	while (it != pass_list.end()){
		if ((*it)->getBusNum() == bus_num) return *it;
		++it;
	}
	return 0;
}

Bus* get_bus_by_num(vector<Bus*> & bus_list, string const & bus_num){
	vector<Bus*>::iterator it = bus_list.begin();
	while (it != bus_list.end()){
		if ((*it)->getNum() == bus_num) return *it;
		++it;
	}
	return 0;
}

void del_bus(vector<Bus*> & bus_list, Bus * bus){
	vector<Bus*>::iterator it = bus_list.begin();
	while (it != bus_list.end()){
		if (*it == bus) {
			bus_list.erase(it);
			return;
		}
		++it;
	}
}

void init_buses(vector<Bus*> & bus_list, int count){
	Bus* bus;
	for (int i = 0; i < count; ++i){
		bus = new Bus(to_string(i % 10 + 1), 30 + rand() % 11);
		bus_list.push_back(bus);
	}
}

void init_passengers(vector<Passenger*> & pass_list, int count){
	Passenger* pass;
	for (int i = 0; i < count; ++i){
		pass = new Passenger("Пассажир" + to_string(i), to_string(1 + rand() % 10), "Главная");
		pass_list.push_back(pass);
	}
}

void input_count(int & n_bus, int & n_pass){
	n_bus = 0;
	n_pass = 0;
	cout << "Введите количество автобусов: ";
	cin >> n_bus;
	cout << "Введите количество пассажиров: ";
	cin >> n_pass;
}

int main(){
	setlocale(LC_ALL, "Russian");
	srand(time(0));
	int avg_bus_capacity = 0;
	map<string, int> pass_count_per_bus;
	vector<Bus*> bus_list, bus_list_print;
	vector<Passenger*> pass_list;
	int n_bus, n_pass;
	input_count(n_bus, n_pass);
	init_buses(bus_list, n_bus);
	init_passengers(pass_list, n_pass);
	bus_list_print = bus_list;
	cout << "Список пассажиров:" << endl;
	for (vector<Passenger*>::iterator it = pass_list.begin(); it != pass_list.end(); ++it){
		pass_count_per_bus[(*it)->getBusNum()]++;
		(*it)->print();
	}
	cout << "Список автобусов:" << endl;
	for (vector<Bus*>::iterator it = bus_list.begin(); it != bus_list.end(); ++it){
		avg_bus_capacity += (*it)->getCapacity();
		(*it)->print();
	}
	avg_bus_capacity /= bus_list.size();
	BusStop bus_stop("Главная");
	bus_stop.setPassengers(pass_list);
	Schedule sched;
	sched.generate(n_bus);
	sched.print();
	system("pause");
	Timer timer;
	timer.setTime(6, 0);
	Bus* bus = 0;
	while (timer.getTime().h < 12){
		timer.tick();
		string curr_bus;
		curr_bus = sched.getBusNumByTime(timer.getTime());
		bus = get_bus_by_num(bus_list, curr_bus);
		if (bus != 0) {
			timer.getTime().print();
			bus->gotoBusStop(bus_stop);
			del_bus(bus_list, bus);
		}
	}
	int bus_pass_count = 0;
	for (vector<Bus*>::iterator it = bus_list_print.begin(); it != bus_list_print.end(); ++it){
		bus_pass_count += (*it)->getPassCount();
		(*it)->print();
		(*it)->printPassengersList();
	}
	bus_stop.printPassengersList();
	cout << "Результаты моделирования:" << endl;
	cout << "Количество пассажиров севших на автобус: " << bus_pass_count << endl;
	cout << "Количество пассажиров, которые не смогли сесть на нужный автобус: " << bus_stop.getPassCount() << endl;
	cout << "Средняя вместимость автобуса: " << avg_bus_capacity << endl;
	cout << "Таблица соотношения вместительности автобуса и количества пассажиров" << endl;
	cout << "---------------------------------------" << endl;
	cout << "   №| Вместительность| Пассажиры|    %|" << endl;
	for (map<string, int>::iterator it = pass_count_per_bus.begin(); it != pass_count_per_bus.end(); ++it){
		int c = get_bus_by_num(bus_list_print, it->first)->getCapacity();
		cout
			<< setw(4) << it->first << "|"
			<< setw(16) << c << "|"
			<< setw(10) << it->second << "|"
			<< setw(5) << (int)(((double) c / it->second) * 100) << "|" << endl;
	}
	cout << "---------------------------------------" << endl;
	system("pause");
	return 0;
}