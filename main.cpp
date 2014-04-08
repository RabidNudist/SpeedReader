#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <Windows.h>
#include <queue>
#include <iomanip>

using namespace std;

//struct that will hold the user properites -- values to be read from a file
struct UserProperties{

	string font_size;
	string chunk_size;
	int wpm;

};

//to be used the first time a user runs the program, creates the properties file
void createFile(){

	ofstream ofs("user.properties");

	if(ofs.good()){
		
		ofs << "font-size:16\n";
		ofs << "chunk-size:1\n";
		ofs << "wpm:400\n";

	}

	ofs.close();
}

//reads the user properties file
UserProperties readFile(){

	UserProperties userp;
	string arr[3];

	ifstream ifs("user.properties");
	if(!ifs.good()){
		createFile();
		ifs.open("user.properties");
	}
	
	string line = "";
	string last = "";
	getline(ifs, line);

	for(int i = 0; line != last; i++){
		int position = line.find(':') + 1;
		arr[i] = line.substr(position, line.length());
		last = line;
		getline(ifs, line);
	}

	userp.font_size = arr[0];
	userp.chunk_size = arr[1];
	userp.wpm = atoi(arr[2].c_str());

	ifs.close();

	return userp;
}

//two variables for using QueryPerformanceCounter as a timer
__int64 start = 0;
float freq = 0;

//starts the timer
void startCounter(){
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);

	freq = float(li.QuadPart)/60000.0;
	QueryPerformanceCounter(&li);
	start = li.QuadPart;
}

//returns the current time of the timer
float getCounter(){

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return float(li.QuadPart - start) / freq;

}


HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

//displays the current word
void display(UserProperties userp, string word){
	system("cls");
	cout << word << endl;
}

//starts the speedreading process
void speedread(UserProperties userp, queue<string> words){

	if(!words.empty())
		display(userp, words.front());

	//set up the timers
	float change_time = getCounter();
	FLOAT change_interval = 1.0f/userp.wpm * 60.0f;

	while(!words.empty()){
		float time = getCounter();
		if(time - change_time >= change_interval){
			words.pop();
			if(!words.empty()){
				display(userp, words.front());
			}
			change_time = getCounter();
		}
	}

}

void seperate(string in, queue<string> &words){
	
	char *str = new char[in.length() + 1];
	strcpy(str, in.c_str());
	
	char* token = strtok(str, " ");

	while(token != NULL){
		words.push(token);
		token = strtok(NULL, " ");
	}

	delete [] str;
	delete [] token;

}

int main(){

	UserProperties userp = readFile();
	queue<string> words;
	string user_input;

	CONSOLE_FONT_INFOEX console_info = {0};

	COORD font = {12, 16};

	console_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	console_info.dwFontSize = font;
	console_info.FontWeight = FW_NORMAL;
	console_info.FontFamily = FF_DONTCARE;
	console_info.nFont = 11;

	SetCurrentConsoleFontEx(console_handle, true, &console_info);

	getline(cin, user_input);
	
	seperate(user_input, words);


	int input = 0;
	startCounter();
	while(input != -1){
	
		switch(input){
		case 0:
			speedread(userp, words);
			break;
		default:
			return 0;
		}

		cout << "Read again?: ";
		cin >> input;
		cout << endl;
	};

}