#ifndef CONSOLEIO_H
#define CONSOLEIO_H

#include <string>
#include <vector>
#include <memory>
#include "Course.h"
#include "Enrollment.h"
#include "CourseManager.h"
using namespace std;

// Thrown when the keyboard input is closed (Ctrl+Z / Ctrl+D), so main() can save and stop.
struct InputClosed{};

//1. MENUS (only print the menu text)
void io_showMainMenu();
void io_showCourseMenu();
void io_showEnrollmentMenu();

//2. SAFE INPUT (each function asks again until the value is valid)
//   Every field has its own function with its own do-while loop and try-catch.
//   The validate...() functions (Course.h / Enrollment.h) throw an exception when the value is wrong.

// ----- text fields (cannot be empty) -----
string io_readCourseTitle(const string &prompt);
string io_readInstructorName(const string &prompt);
string io_readLearnerName(const string &prompt);
string io_readKeyword(const string &prompt);	// search keyword
string io_readDate(const string &prompt);	// YYYY-MM-DD

// ----- number fields -----
double io_readDuration(const string &prompt);
double io_readTuitionFee(const string &prompt);
double io_readProgress(const string &prompt);
double io_readFinalScore(const string &prompt);

// ----- optional fields for the update screen (empty = keep the old value) -----
string io_readOptionalTitle(const string &prompt);	// returns "" when the user presses Enter
string io_readOptionalInstructor(const string &prompt);	// returns "" when the user presses Enter
bool   io_readOptionalDuration(const string &prompt, double &value);	// false = empty
bool   io_readOptionalTuitionFee(const string &prompt, double &value);	// false = empty

// ----- other input -----
int    io_readChoice(const string &prompt, int minValue, int maxValue);	// an integer in a range
bool   io_confirm(const string &question);	// yes / no
string io_readCourseId(const CourseManager &manager, const string &prompt);	// existing course ID, "" = cancel
string io_readEnrollmentId(const CourseManager &manager, const string &prompt);	// existing enrollment ID, "" = cancel

//3. DISPLAY (print a list as a table)
void io_displayCourses(const vector<shared_ptr<Course> > &courses, const string &title);
void io_displayEnrollments(const vector<shared_ptr<Enrollment> > &enrollments, const string &title);

//4. SCREENS: read the input, call the manager, print the result
void io_courseMenu(CourseManager &manager);	// Course Management screen
void io_enrollmentMenu(CourseManager &manager);	// Enrollment Management screen
void io_showReport(const CourseManager &manager);	// number of learners per course
void io_showLoadResult(const CourseManager &manager, int skippedLines);
void io_saveData(const CourseManager &manager);

#endif
