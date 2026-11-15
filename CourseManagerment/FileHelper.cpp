#include "FileHelper.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

// Splits "a|b|c" into the three parts "a", "b" and "c".
static vector<string> splitLine(const string &line){
	vector<string> parts;
	stringstream stream(line);
	string part;
	// Loop until there is no more part in the line
	while(getline(stream, part, '|')){
		parts.push_back(part);
	}
	return parts;
}

// Reads ONE line of courses.txt: Type|ID|Title|Instructor|Hours|Fee
// Throws an exception when the line is wrong.
static void loadCourseLine(CourseManager &manager, const string &line){
	vector<string> p = splitLine(line);
	if(p.size() != 6){
		throw invalid_argument("wrong number of fields");
	}

	double hours = parseNumber(p[4], "Hours");
	double fee = parseNumber(p[5], "Fee");

	if(p[0] == "Free"){
		if(fee != 0){
			throw invalid_argument("a Free course must have fee 0");
		}
		manager.addCourse(make_shared<FreeCourse>(p[1], p[2], p[3], hours));
	}else if(p[0] == "Paid"){
		manager.addCourse(make_shared<PaidCourse>(p[1], p[2], p[3], hours, fee));
	}else{
		throw invalid_argument("unknown course type");
	}
}

// Reads ONE line of enrollments.txt: ID|Name|CourseID|Date|Progress|Score|Certificate
// Throws an exception when the line is wrong.
static void loadEnrollmentLine(CourseManager &manager, const string &line){
	vector<string> p = splitLine(line);
	if(p.size() != 7){
		throw invalid_argument("wrong number of fields");
	}
	if(!manager.findCourse(p[2])){
		throw out_of_range("the course of this enrollment does not exist");
	}
	if(p[6] != "0" && p[6] != "1"){
		throw invalid_argument("certificate must be 0 or 1");
	}

	// The same learner cannot appear twice in the same course.
	const vector<shared_ptr<Enrollment> > &old = manager.getEnrollments();
	int oldCount = old.size();
	for(int i = 0; i < oldCount; i++){
		if(old[i]->getCourseId() == p[2] && old[i]->getLearnerName() == p[1]){
			throw runtime_error("duplicated enrollment");
		}
	}

	shared_ptr<Enrollment> enrollment = make_shared<Enrollment>(p[0], p[1], p[2], p[3]);
	enrollment->setProgress(parseNumber(p[4], "Progress"));
	enrollment->setFinalScore(parseNumber(p[5], "Score"));
	enrollment->setCertificateIssued(p[6] == "1");
	manager.addEnrollment(enrollment);
}

int file_load(CourseManager &manager){
	int skipped = 0;	// number of wrong lines
	string line;

	// ----- courses first (an enrollment needs its course) -----
	ifstream courseFile(COURSE_FILE.c_str());
	// Loop until the last line of courses.txt (if the file does not exist, the loop does nothing)
	while(getline(courseFile, line)){
		if(line == ""){
			continue;
		}
		// Try to load the line; a wrong line is counted and skipped
		try{
			loadCourseLine(manager, line);
		}catch(const exception&){
			skipped++;
		}
	}

	// ----- then the enrollments -----
	ifstream enrollmentFile(ENROLLMENT_FILE.c_str());
	// Loop until the last line of enrollments.txt
	while(getline(enrollmentFile, line)){
		if(line == ""){
			continue;
		}
		// Try to load the line; a wrong line is counted and skipped
		try{
			loadEnrollmentLine(manager, line);
		}catch(const exception&){
			skipped++;
		}
	}
	return skipped;
}

bool file_save(const CourseManager &manager){
	ofstream courseFile(COURSE_FILE.c_str());
	ofstream enrollmentFile(ENROLLMENT_FILE.c_str());
	if(!courseFile || !enrollmentFile){
		return false;	// a file could not be opened
	}

	const vector<shared_ptr<Course> > &courses = manager.getCourses();
	int courseCount = courses.size();
	for(int i = 0; i < courseCount; i++){
		courseFile << courses[i]->toFileLine() << "\n";
	}

	const vector<shared_ptr<Enrollment> > &enrollments = manager.getEnrollments();
	int enrollmentCount = enrollments.size();
	for(int i = 0; i < enrollmentCount; i++){
		enrollmentFile << enrollments[i]->toFileLine() << "\n";
	}

	return courseFile.good() && enrollmentFile.good();
}
