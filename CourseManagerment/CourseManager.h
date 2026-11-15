#ifndef COURSEMANAGER_H
#define COURSEMANAGER_H

#include <vector>
#include <map>
#include <memory>
#include <string>
#include "Course.h"
#include "Enrollment.h"
using namespace std;

//1 - SMALL COMPARISON FUNCTIONS
//	The template functions below do not know HOW to compare two items.
//	We give them one of these small functions with a clear name.

// ----- "Does this item match the keyword?"  (used for searching) -----
bool hasCourseId(const shared_ptr<Course> &course, const string &id);
bool hasCourseType(const shared_ptr<Course> &course, const string &type);
bool isTitleOrInstructorMatch(const shared_ptr<Course> &course, const string &keyword);
bool hasEnrollmentId(const shared_ptr<Enrollment> &enrollment, const string &id);
bool isLearnerNameMatch(const shared_ptr<Enrollment> &enrollment, const string &keyword);

// ----- "Should a come before b?"  (used for sorting) -----
bool isCheaper(const shared_ptr<Course> &a, const shared_ptr<Course> &b);
bool hasHigherScore(const shared_ptr<Enrollment> &a, const shared_ptr<Enrollment> &b);

//2 - TEMPLATE FUNCTIONS (Data Analysis)
//  "template <typename T>" means: the same code works for Course AND
//  Enrollment. T is replaced by the real type when we call the function.
//  The last parameter is a pointer to one of the small functions above.

// Returns the FIRST item that matches (search by ID).
// An empty pointer means "not found".
template <typename T>
shared_ptr<T> findFirst(const vector<shared_ptr<T> > &items, const string &keyword, bool (*isMatch)(const shared_ptr<T>&, const string&)){
	int count = items.size();
	for(int i = 0; i < count; i++){
		if(isMatch(items[i], keyword)){
			return items[i];
		}
	}
	return shared_ptr<T>();
}

// Returns ALL items that match (search by name / keyword).
template <typename T>
vector<shared_ptr<T> > searchBy(const vector<shared_ptr<T> > &items, const string &keyword, bool (*isMatch)(const shared_ptr<T>&, const string&)){
	vector<shared_ptr<T> > result;
	int count = items.size();
	for(int i = 0; i < count; i++){
		if(isMatch(items[i], keyword)){
			result.push_back(items[i]);
		}
	}
	return result;
}

// Sorts the list with bubble sort (the same algorithm as in the labs).
// comesFirst(a, b) tells us whether a must be placed before b.
template <typename T>
void sortBy(vector<shared_ptr<T> > &items, bool (*comesFirst)(const shared_ptr<T>&, const shared_ptr<T>&)){
	int count = items.size();
	for(int i = 0; i < count - 1; i++){
		for(int j = 0; j < count - i - 1; j++){
			if(comesFirst(items[j + 1], items[j])){
				shared_ptr<T> temp = items[j];	// swap the two neighbours
				items[j] = items[j + 1];
				items[j + 1] = temp;
			}
		}
	}
}

//3 - CourseManager: holds all the data and the business rules
class CourseManager{
private:
	vector<shared_ptr<Course> > courses;	// all courses
	vector<shared_ptr<Enrollment> > enrollments;	// all enrollments
	map<string, int> enrollmentCount;	// courseId -> number of learners

	int nextCourseNumber;	// next number for CRSxxx
	int nextEnrollmentNumber;	// next number for ENRxxx

	string makeId(const string &prefix, int number) const;
	bool isAlreadyEnrolled(const string &learnerName, const string &courseId) const;

public:
	CourseManager();

	// ----- ID generation -----
	string generateCourseId() const;
	string generateEnrollmentId() const;

	// ----- Course management -----
	void addCourse(shared_ptr<Course> course);
	void deleteCourse(const string &courseId);
	shared_ptr<Course> findCourse(const string& courseId) const;
	vector<shared_ptr<Course> > searchCourses(const string &keyword) const;
	vector<shared_ptr<Course> > getCoursesByType(const string &type) const;
	vector<shared_ptr<Course> > getCoursesSortedByFee() const;

	// ----- Enrollment management -----
	void addEnrollment(shared_ptr<Enrollment> enrollment);	// also used when loading the file
	shared_ptr<Enrollment> enrollLearner(const string &learnerName, const string &courseId, const string &date);
	void deleteEnrollment(const string &enrollmentId);
	shared_ptr<Enrollment> findEnrollment(const string &enrollmentId) const;
	vector<shared_ptr<Enrollment> > searchEnrollments(const string &keyword) const;
	vector<shared_ptr<Enrollment> > getEnrollmentsSortedByScore() const;
	bool updateProgress(const string &enrollmentId, double progress, double score);

	// ----- Read-only access for the other modules -----
	const vector<shared_ptr<Course> > &getCourses() const;
	const vector<shared_ptr<Enrollment> > &getEnrollments() const;
	const map<string, int> &getEnrollmentCount() const;
};

#endif
