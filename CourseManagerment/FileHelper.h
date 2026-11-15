#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <string>
#include "CourseManager.h"
using namespace std;

const string COURSE_FILE = "courses.txt";
const string ENROLLMENT_FILE = "enrollments.txt";

// Reads both files into the manager. A missing file is fine on the first run.
// Returns the number of lines that were skipped because they were invalid.
int file_load(CourseManager &manager);

// Writes all courses and enrollments. Returns true on success.
bool file_save(const CourseManager &manager);

#endif
