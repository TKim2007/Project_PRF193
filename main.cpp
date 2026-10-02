#include <iostream>
#include <iomanip>
#include <limits>
#include "CourseManager.h"
#include "FileManager.h"
#include "Exceptions.h"

using namespace std;

int main(){
  int choice;
      do {
          cout << "\n=========================================\n"
                    << "  HE THONG QUAN LY KHOA HOC TRUC TUYEN\n"
                    << "=========================================\n"
                    << "1. Quan ly khoa hoc\n"
                    << "2. Quan ly dang ky hoc\n"
                    << "0. Luu & Thoat\n";
          choice = readInt("Lua chon: ");
          switch (choice) {
              case 1: courseMenu(manager); break;
              case 2: enrollmentMenu(manager); break;
              case 0:
                  FileManager::saveData(manager, COURSE_FILE, ENROLL_FILE);
                  cout << "Da luu du lieu. Tam biet!\n";
                  break;
              default: std::cout << "Lua chon khong hop le.\n";
          }
      } while (choice != 0);
    return 0;
}
