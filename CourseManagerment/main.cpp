#include <iostream>
#include "ConsoleIO.h"
#include "FileHelper.h"
#include "CourseManager.h"
using namespace std;

int main(){
	CourseManager manager;

	// Load data automatically when the program starts
	int skippedLines = file_load(manager);
	io_showLoadResult(manager, skippedLines);

	try{
		int choice;
		do{
			io_showMainMenu();
			choice = io_readChoice("Enter your choice: ", 0, 4);
			switch(choice){
				case 1:
					io_courseMenu(manager);
					break;
				case 2:
					io_enrollmentMenu(manager);
					break;
				case 3:
					io_showReport(manager);
					break;
				case 4:
					io_saveData(manager);
					break;
				case 0:	// save automatically when the program exits
					io_saveData(manager);
					cout << "Goodbye!\n";
					break;
			}
		}while(choice != 0);
	}catch(const InputClosed&){
		// the keyboard input was closed: save the data and stop
		cout << "\nInput closed. Saving data...\n";
		io_saveData(manager);
	}
	return 0;
}