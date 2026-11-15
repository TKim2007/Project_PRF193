#include "Course.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

// 1 - SMALL HELPERS
// A letter: only the English letters a-z and A-Z.
static bool isLetter(char c){
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return true;
	}
	return false;
}

// A normal character on the keyboard: ASCII code from 32 (space) to 126 (~).
static bool isPrintableChar(char c){
	if(c >= 32 && c <= 126){
		return true;
	}
	return false;
}

// Checks the format PPPddd: 3 fixed letters followed by 3 digits (CRS001, ENR012...).
static void checkIdFormat(const string &id, const string &prefix, const string &fieldName){
	bool ok = true;
	if(id.length() != 6){
		ok = false;
	}else{
		if(id.substr(0, 3) != prefix){
			ok = false;
		}
		for(int i = 3; i < 6; i++){
			if(id[i] < '0' || id[i] > '9'){
				ok = false;
			}
		}
	}
	if(!ok){
		throw invalid_argument(fieldName + " must have the format " + prefix + "xxx (example: " + prefix + "001)");
	}
}

// 2 - VALIDATION FUNCTIONS
double parseNumber(const string &text, const string &fieldName){
	if(text == ""){
		throw invalid_argument(fieldName + " cannot be empty");
	}
	istringstream in(text);
	double value;
	in >> value;
	if(in.fail()){
		throw invalid_argument(fieldName + " must be a number");
	}
	char extra;
	if(in >> extra){	// something is left after the number, for example "12abc"
		throw invalid_argument(fieldName + " must be a number (no extra characters)");
	}
	return value;
}

void checkPersonName(const string &text, const string &fieldName){
	if(text == ""){
		throw invalid_argument(fieldName + " cannot be empty");
	}
	if(text.length() > 50){
		throw invalid_argument(fieldName + " cannot be longer than 50 characters");
	}

	bool hasLetter = false;
	int length = text.length();
	for(int i = 0; i < length; i++){
		char c = text[i];
		if(isLetter(c)){
			hasLetter = true;
		}else if(c == ' ' || c == '.' || c == '-' || c == '\''){
			// allowed separators (example: "Dr. Hoang", "O'Neil", "Anh-Tuan")
		}else{
			throw invalid_argument(fieldName + " can only contain letters, spaces and . - ' (no digits or other symbols)");
		}
	}
	if(!hasLetter){
		throw invalid_argument(fieldName + " must contain at least one letter");
	}
}

void validateCourseId(const string &id){
	checkIdFormat(id, "CRS", "Course ID");
}

void validateTitle(const string &title){
	if(title == ""){
		throw invalid_argument("Course title cannot be empty");
	}
	if(title.length() > 60){
		throw invalid_argument("Course title cannot be longer than 60 characters");
	}
	bool hasLetter = false;
	int length = title.length();
	for(int i = 0; i < length; i++){
		if(title[i] == '|'){
			throw invalid_argument("Course title cannot contain the character '|'");
		}
		if(!isPrintableChar(title[i])){
			throw invalid_argument("Course title contains an invalid character (use English letters, digits and normal symbols only)");
		}
		if(isLetter(title[i])){
			hasLetter = true;
		}
	}
	if(!hasLetter){
		throw invalid_argument("Course title must contain at least one letter");
	}
}

void validateInstructorName(const string &name){
	checkPersonName(name, "Instructor name");
}

void validateDuration(double hours){
	if(hours <= 0){
		throw invalid_argument("Duration must be greater than 0");
	}
	if(hours > 1000){
		throw invalid_argument("Duration cannot be more than 1000 hours");
	}
}

void validateTuitionFee(double fee){
	if(fee < 0){
		throw invalid_argument("Tuition fee cannot be negative");
	}
	if(fee > 1000000000){
		throw invalid_argument("Tuition fee is too large (maximum 1000000000)");
	}
}

// 3 - Course (base class)
Course::Course(const string &id, const string &title, const string &instructor, double durationHours, double tuitionFee){
	validateCourseId(id);
	this->courseId = id;
	setTitle(title);
	setInstructor(instructor);
	setDurationHours(durationHours);

	validateTuitionFee(tuitionFee);
	this->tuitionFee = tuitionFee;
}

string Course::getCourseId() const{ return courseId; }
string Course::getTitle() const{ return title; }
string Course::getInstructor() const{ return instructor; }
double Course::getDurationHours() const{ return durationHours; }
double Course::getTuitionFee() const{ return tuitionFee; }

void Course::setTitle(const string &newTitle){
	validateTitle(newTitle);
	title = newTitle;
}

void Course::setInstructor(const string &newInstructor){
	validateInstructorName(newInstructor);
	instructor = newInstructor;
}

void Course::setDurationHours(double hours){
	validateDuration(hours);
	durationHours = hours;
}

void Course::setTuitionFee(double fee){
	validateTuitionFee(fee);
	tuitionFee = fee;
}

string Course::toFileLine() const{
	// ostringstream works like cout, but it builds a string instead of printing
	ostringstream line;
	line << setprecision(10);	// keep big numbers in normal form (not 1e+06)
	line << getType() << "|" << courseId << "|" << title << "|" << instructor << "|" << durationHours << "|" << tuitionFee;
	return line.str();
}

// 4 - FreeCourse
FreeCourse::FreeCourse(const string &id, const string &title, const string &instructor, double durationHours) : Course(id, title, instructor, durationHours, 0){}	// the fee is always 0

void FreeCourse::setTuitionFee(double fee){
	// A free course must stay free, so every change of the fee is refused.
	throw invalid_argument("A Free course always has tuition fee = 0");
}

string FreeCourse::getType() const{
	return "Free";
}

bool FreeCourse::isEligibleForCertificate(double progress, double score) const{
	// The score is not needed for a free course, only the progress.
	if(progress >= 100){
		return true;
	}
	return false;
}

// 5 - PaidCourse
PaidCourse::PaidCourse(const string &id, const string &title, const string &instructor, double durationHours, double tuitionFee) : Course(id, title, instructor, durationHours, tuitionFee){}

string PaidCourse::getType() const{
	return "Paid";
}

bool PaidCourse::isEligibleForCertificate(double progress, double score) const{
	const double MIN_PROGRESS = 80;
	const double PASSING_SCORE = 50;
	if(progress >= MIN_PROGRESS && score >= PASSING_SCORE){
		return true;
	}
	return false;
}
