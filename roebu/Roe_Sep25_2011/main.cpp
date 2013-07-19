

//*
#include "CApp.h"
#include <iostream>
#include <functional>
#include <algorithm>
using namespace std;
using namespace roe;


class id_generator {
public:
	id_generator(int numBytes, int min = 0) {
		start = min;
		bytes.resize(numBytes);
	}
	~id_generator() {bytes.clear();}
	
	int reserve() { //reserves first free ID
		for(int i = 0;;++i) {
			if(!isReserved(i)) {
				reserve(i);
				return i+start;
			}
		}
		return start-1; //error
	}
	void free(int index) { //frees index as ID
		assert(index >= start);
		set(index-start, false);
	}
private:
	bool isReserved(int index) {return get(index);}
	void reserve(int index) {set(index, true);}
	void set(int index, bool used) {
		if (used) bytes[index/8] |= (1<<(index%8));
		else bytes[index/8] &= 255 - (1<<(index%8));
	}
	bool get(int index) {
		return !(bytes[index/8] & (1<<(index%8)));
	}
	
	std::vector<unsigned char> bytes;
	int start; //first allowed
};

int main(int argc, char **argv) {
	
	
#ifdef BULLET_GIMPACT
	//cout << "hello" << endl;
#endif
	
	CApp *app = NULL;
	try {
		
			
		// Create application object
		app = new CApp();
		
		app->run();
	
	}
	catch(const roe::Exception& e){
		roe_log << e;
		cout << e.toString() << endl;
	}
	
	if (app != NULL) delete app;
	
	util::println("right quitting!!!");
	
	return 0;
	
}
//*/

