#include "Enrollment.h"
#include "Course.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

// 1 - VALIDATION FUNCTIONS
void validateEnrollmentId(const string &id){
	bool ok = true;
	if(id.length() != 6 || id.substr(0, 3) != "ENR"){
		ok = false;
	}else{
		for(int i = 3; i < 6; i++){
			if(id[i] < '0' || id[i] > '9'){
				ok = false;
			}
		}
	}
	if(!ok){
		throw invalid_argument("Enrollment ID must have the format ENRxxx (example: ENR001)");
	}
}

void validateLearnerName(const string &name){
	checkPersonName(name, "Learner name");
}

// Checks the format YYYY-MM-DD AND that the date really exists (no 2026-02-30).
void validateDate(const string &date){
	if(date.length() != 10 || date[4] != '-' || date[7] != '-'){
		throw invalid_argument("Date must be in the format YYYY-MM-DD (example: 2026-09-15)");
	}
	for(int i = 0; i < 10; i++){
		if(i == 4 || i == 7){
			continue;	// skip the two '-'
		}
		if(date[i] < '0' || date[i] > '9'){
			throw invalid_argument("Date must be in the format YYYY-MM-DD (digits only)");
		}
	}

	int year = stoi(date.substr(0, 4));
	int month = stoi(date.substr(5, 2));
	int day = stoi(date.substr(8, 2));

	if(year < 2000 || year > 2100){
		throw invalid_argument("Year must be between 2000 and 2100");
	}
	if(month < 1 || month > 12){
		throw invalid_argument("Month must be between 01 and 12");
	}

	int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	if(leapYear){
		daysInMonth[1] = 29;
	}
	if(day < 1 || day > daysInMonth[month - 1]){
		throw invalid_argument("Day is not valid for this month");
	}
}

void validateProgress(double progress){
	if(progress < 0 || progress > 100){
		throw invalid_argument("Progress must be between 0 and 100");
	}
}

void validateFinalScore(double score){
	if(score < 0 || score > 100){
		throw invalid_argument("Final score must be between 0 and 100");
	}
}

// 2 - Enrollment
Enrollment::Enrollment(const string &id, const string &learnerName, const string &courseId, const string &date){
	validateEnrollmentId(id);
	validateLearnerName(learnerName);
	validateCourseId(courseId);
	validateDate(date);

	this->enrollmentId = id;
	this->learnerName = learnerName;
	this->courseId = courseId;
	this->enrollmentDate = date;
	this->progress = 0;	// a new learner starts with nothing
	this->finalScore = 0;
	this->certificateIssued = false;
}

string Enrollment::getEnrollmentId() const{ return enrollmentId; }
string Enrollment::getLearnerName() const{ return learnerName; }
string Enrollment::getCourseId() const{ return courseId; }
string Enrollment::getEnrollmentDate() const{ return enrollmentDate; }
double Enrollment::getProgress() const{ return progress; }
double Enrollment::getFinalScore() const{ return finalScore; }
bool Enrollment::isCertificateIssued() const{ return certificateIssued; }

void Enrollment::setProgress(double newProgress){
	validateProgress(newProgress);
	progress = newProgress;
}

void Enrollment::setFinalScore(double newScore){
	validateFinalScore(newScore);
	finalScore = newScore;
}

void Enrollment::setCertificateIssued(bool issued){
	certificateIssued = issued;
}

string Enrollment::toFileLine() const{
	ostringstream line;
	line << setprecision(10);
	line << enrollmentId << "|" << learnerName << "|" << courseId << "|" << enrollmentDate << "|" << progress << "|" << finalScore << "|";
	if(certificateIssued){
		line << 1;
	}else{
		line << 0;
	}
	return line.str();
}
