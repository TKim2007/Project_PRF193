#ifndef COURSE_H
#define COURSE_H

#include <string>
using namespace std;

//==================================================================
// VALIDATION FUNCTIONS
// Every function below checks ONE value and throws an exception
// (invalid_argument) with a clear message when the value is wrong.
// They are used by the classes AND by the console screens, so the
// same rule is applied everywhere (typing, loading a file, ...).
//==================================================================
double parseNumber(const string &text, const string &fieldName);	// text -> number, throws if text is not a number
void checkPersonName(const string &text, const string &fieldName);	// English letters, spaces, '.', '-', '\'' only (ASCII)
void validateCourseId(const string &id);	// format CRSxxx
void validateTitle(const string &title);	// printable ASCII, no '|', at least one letter
void validateInstructorName(const string &name);
void validateDuration(double hours);	// 0 < hours <= 1000
void validateTuitionFee(double fee);	// 0 <= fee <= 1000000000

/**
 * Course = abstract base class (it has pure virtual functions, so we cannot
 * create a plain Course object, only a FreeCourse or a PaidCourse).
 * All data members are private; other code uses getters and setters.
 */
class Course {
private:
	string courseId;	// unique, format CRSxxx
	string title;
	string instructor;
	double durationHours;	// must be > 0
	double tuitionFee;	// must be >= 0

public:
	Course(const string& id, const string &title, const string &instructor, double durationHours, double tuitionFee);
	virtual ~Course(){}

	// ----- Getters -----
	string getCourseId() const;
	string getTitle() const;
	string getInstructor() const;
	double getDurationHours() const;
	double getTuitionFee() const;

	// ----- Setters (check the new value, throw an exception if wrong) -----
	void setTitle(const string &newTitle);
	void setInstructor(const string &newInstructor);
	void setDurationHours(double hours);
	virtual void setTuitionFee(double fee);	// virtual: FreeCourse changes it

	// ----- Each course type has its own version (polymorphism) -----
	virtual string getType() const = 0;	// returns "Free" or "Paid"
	virtual bool isEligibleForCertificate(double progress, double score) const = 0;

	// One line of the data file: Type|ID|Title|Instructor|Hours|Fee
	string toFileLine() const;
};

/**
 * FreeCourse: the fee is always 0.
 * Certificate rule: progress = 100%.
 */
class FreeCourse : public Course{
public:
	FreeCourse(const string &id, const string &title, const string &instructor, double durationHours);

	virtual void setTuitionFee(double fee);
	virtual string getType() const;
	virtual bool isEligibleForCertificate(double progress, double score) const;
};

/**
 * PaidCourse: has a tuition fee.
 * Certificate rule: progress >= 80% AND final score >= 50.
 */
class PaidCourse : public Course{
public:
	PaidCourse(const string &id, const string &title, const string &instructor, double durationHours, double tuitionFee);

	virtual string getType() const;
	virtual bool isEligibleForCertificate(double progress, double score) const;
};

#endif
