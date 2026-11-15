#include "CourseManager.h"
#include <stdexcept>
#include <cctype>
using namespace std;

// Exceptions used in this file (all are standard C++ exceptions):
//   invalid_argument : a value is wrong (outside 0-100, empty name, bad format...)
//   out_of_range     : an ID does not exist (course / enrollment not found)
//   runtime_error    : a rule forbids the action (duplicate, course still has learners)

// Makes a copy of the text in lower case (used for searching without caring about A/a).
static string toLower(const string &text){
	string result = text;
	int length = result.length();
	for(int i = 0; i < length; i++){
		result[i] = (char)tolower((unsigned char)result[i]);
	}
	return result;
}

//1 - SMALL COMPARISON FUNCTIONS
bool hasCourseId(const shared_ptr<Course> &course, const string &id){
	return course->getCourseId() == id;
}

bool hasCourseType(const shared_ptr<Course> &course, const string &type){
	return course->getType() == type;
}

// The keyword can be a part of the title OR a part of the instructor name.
bool isTitleOrInstructorMatch(const shared_ptr<Course> &course, const string &keyword){
	string key = toLower(keyword);
	bool inTitle = toLower(course->getTitle()).find(key) != string::npos;
	bool inInstructor = toLower(course->getInstructor()).find(key) != string::npos;
	if(inTitle || inInstructor){
		return true;
	}
	return false;
}

bool hasEnrollmentId(const shared_ptr<Enrollment> &enrollment, const string &id){
	return enrollment->getEnrollmentId() == id;
}

bool isLearnerNameMatch(const shared_ptr<Enrollment> &enrollment, const string &keyword){
	string name = toLower(enrollment->getLearnerName());
	return name.find(toLower(keyword)) != string::npos;
}

// Ascending fee: the cheaper course comes first.
bool isCheaper(const shared_ptr<Course> &a, const shared_ptr<Course> &b){
	return a->getTuitionFee() < b->getTuitionFee();
}

// Descending score: the higher score comes first.
bool hasHigherScore(const shared_ptr<Enrollment> &a, const shared_ptr<Enrollment> &b){
	return a->getFinalScore() > b->getFinalScore();
}

//2 - CourseManager: constructor and ID generation
CourseManager::CourseManager(){
	nextCourseNumber = 1;
	nextEnrollmentNumber = 1;
}

// makeId("CRS", 7) returns "CRS007"
string CourseManager::makeId(const string &prefix, int number) const{
	string digits = to_string(number);
	while(digits.length() < 3){
		digits = "0" + digits;
	}
	return prefix + digits;
}

string CourseManager::generateCourseId() const{
	if(nextCourseNumber > 999){
		throw runtime_error("The system is full: the maximum course ID is CRS999");
	}
	return makeId("CRS", nextCourseNumber);
}

string CourseManager::generateEnrollmentId() const{
	if(nextEnrollmentNumber > 999){
		throw runtime_error("The system is full: the maximum enrollment ID is ENR999");
	}
	return makeId("ENR", nextEnrollmentNumber);
}

//3 - COURSE MANAGEMENT
shared_ptr<Course> CourseManager::findCourse(const string &courseId) const{
	return findFirst<Course>(courses, courseId, hasCourseId);
}

void CourseManager::addCourse(shared_ptr<Course> course){
	if(!course){
		throw invalid_argument("Course is empty");
	}
	string id = course->getCourseId();

	if(findCourse(id)){
		throw runtime_error("Course ID already exists: " + id);
	}

	courses.push_back(course);
	enrollmentCount[id] = 0;	// a new course has no learner yet

	// Keep the counter ahead of every ID in use, so a new ID is never reused.
	int number = stoi(id.substr(3));	// "CRS007" -> 7
	if(number >= nextCourseNumber){
		nextCourseNumber = number + 1;
	}
}

void CourseManager::deleteCourse(const string &courseId){
	shared_ptr<Course> course = findCourse(courseId);
	if(!course){
		throw out_of_range("Course not found: " + courseId);
	}

	// Business rule: a course that still has learners cannot be deleted.
	if(enrollmentCount[courseId] > 0){
		throw runtime_error("Cannot delete course " + courseId + ": it still has " + to_string(enrollmentCount[courseId]) + " enrolled learner(s)");
	}

	// Find the course in the list and remove it.
	int count = courses.size();
	for(int i = 0; i < count; i++){
		if(courses[i]->getCourseId() == courseId){
			courses.erase(courses.begin() + i);
			break;
		}
	}
	enrollmentCount.erase(courseId);
}

vector<shared_ptr<Course> > CourseManager::searchCourses(const string &keyword) const{
	return searchBy<Course>(courses, keyword, isTitleOrInstructorMatch);
}

vector<shared_ptr<Course> > CourseManager::getCoursesByType(const string &type) const{
	return searchBy<Course>(courses, type, hasCourseType);
}

vector<shared_ptr<Course> > CourseManager::getCoursesSortedByFee() const{
	vector<shared_ptr<Course> > sorted = courses;	// work on a copy
	sortBy<Course>(sorted, isCheaper);
	return sorted;
}

//4 - ENROLLMENT MANAGEMENT
shared_ptr<Enrollment> CourseManager::findEnrollment(const string &enrollmentId) const{
	return findFirst<Enrollment>(enrollments, enrollmentId, hasEnrollmentId);
}

void CourseManager::addEnrollment(shared_ptr<Enrollment> enrollment){
	if(!enrollment){
		throw invalid_argument("Enrollment is empty");
	}
	string id = enrollment->getEnrollmentId();

	if(findEnrollment(id)){
		throw runtime_error("Enrollment ID already exists: " + id);
	}
	if(!findCourse(enrollment->getCourseId())){
		throw out_of_range("Course not found: " + enrollment->getCourseId());
	}

	enrollments.push_back(enrollment);
	enrollmentCount[enrollment->getCourseId()]++;	// one more learner in this course

	int number = stoi(id.substr(3));	// "ENR007" -> 7
	if(number >= nextEnrollmentNumber){
		nextEnrollmentNumber = number + 1;
	}
}

// True when this learner already joined this course (the capital letters do not matter).
bool CourseManager::isAlreadyEnrolled(const string &learnerName, const string &courseId) const{
	int count = enrollments.size();
	for(int i = 0; i < count; i++){
		bool sameCourse = enrollments[i]->getCourseId() == courseId;
		bool sameLearner = toLower(enrollments[i]->getLearnerName()) == toLower(learnerName);
		if(sameCourse && sameLearner){
			return true;
		}
	}
	return false;
}

shared_ptr<Enrollment> CourseManager::enrollLearner(const string &learnerName, const string &courseId, const string &date){
	// 1. Check the values first.
	validateLearnerName(learnerName);
	validateCourseId(courseId);
	validateDate(date);

	// 2. The course must exist.
	if(!findCourse(courseId)){
		throw out_of_range("Course not found: " + courseId);
	}

	// 3. The same learner cannot join the same course twice.
	if(isAlreadyEnrolled(learnerName, courseId)){
		throw runtime_error(learnerName + " is already enrolled in course " + courseId);
	}

	// 4. Create the enrollment.
	shared_ptr<Enrollment> enrollment = make_shared<Enrollment>(generateEnrollmentId(), learnerName, courseId, date);

	// 5. Store it (this also adds 1 to the learner count of the course).
	addEnrollment(enrollment);
	return enrollment;
}

void CourseManager::deleteEnrollment(const string &enrollmentId){
	shared_ptr<Enrollment> enrollment = findEnrollment(enrollmentId);
	if(!enrollment){
		throw out_of_range("Enrollment not found: " + enrollmentId);
	}

	enrollmentCount[enrollment->getCourseId()]--;	// one learner less in this course

	int count = enrollments.size();
	for(int i = 0; i < count; i++){
		if(enrollments[i]->getEnrollmentId() == enrollmentId){
			enrollments.erase(enrollments.begin() + i);
			break;
		}
	}
}

vector<shared_ptr<Enrollment> > CourseManager::searchEnrollments(const string &keyword) const{
	return searchBy<Enrollment>(enrollments, keyword, isLearnerNameMatch);
}

vector<shared_ptr<Enrollment> > CourseManager::getEnrollmentsSortedByScore() const{
	vector<shared_ptr<Enrollment> > sorted = enrollments;	// work on a copy
	sortBy<Enrollment>(sorted, hasHigherScore);
	return sorted;
}

// Updates progress and score. Returns true if a certificate is issued by this update.
bool CourseManager::updateProgress(const string &enrollmentId, double progress, double score){
	shared_ptr<Enrollment> enrollment = findEnrollment(enrollmentId);
	if(!enrollment){
		throw out_of_range("Enrollment not found: " + enrollmentId);
	}

	shared_ptr<Course> course = findCourse(enrollment->getCourseId());
	if(!course){
		throw out_of_range("Course not found: " + enrollment->getCourseId());
	}

	// Check BOTH values first, so a wrong input never changes only one of them.
	validateProgress(progress);
	validateFinalScore(score);

	enrollment->setProgress(progress);
	enrollment->setFinalScore(score);

	// Polymorphism: course is a FreeCourse or a PaidCourse, and each one
	// has its own isEligibleForCertificate(). We do not need to check the type.
	bool eligible = course->isEligibleForCertificate(progress, score);
	if(eligible && !enrollment->isCertificateIssued()){
		enrollment->setCertificateIssued(true);
		return true;
	}
	return false;
}

//5 - READ-ONLY ACCESS
const vector<shared_ptr<Course> > &CourseManager::getCourses() const{
	return courses;
}

const vector<shared_ptr<Enrollment> > &CourseManager::getEnrollments() const{
	return enrollments;
}

const map<string, int> &CourseManager::getEnrollmentCount() const{
	return enrollmentCount;
}
