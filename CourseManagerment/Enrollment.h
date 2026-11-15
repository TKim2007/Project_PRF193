#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include <string>
using namespace std;

//==================================================================
// VALIDATION FUNCTIONS (throw invalid_argument when the value is wrong)
//==================================================================
void validateEnrollmentId(const string &id);	// format ENRxxx
void validateLearnerName(const string &name);	// letters and spaces only
void validateDate(const string &date);	// real date, format YYYY-MM-DD
void validateProgress(double progress);	// 0 - 100
void validateFinalScore(double score);	// 0 - 100

/**
 * Enrollment = one learner registered in one course.
 * All data members are private; other code uses getters and setters.
 */
class Enrollment{
private:
	string enrollmentId;	// unique, format ENRxxx
	string learnerName;
	string courseId;	// tells us which course the learner joined
	string enrollmentDate;	// YYYY-MM-DD
	double progress;	// 0 - 100 (%)
	double finalScore;	// 0 - 100
	bool certificateIssued;

public:
	Enrollment(const string &id, const string &learnerName, const string &courseId, const string &date);

	// ----- Getters -----
	string getEnrollmentId() const;
	string getLearnerName() const;
	string getCourseId() const;
	string getEnrollmentDate() const;
	double getProgress() const;
	double getFinalScore() const;
	bool isCertificateIssued() const;

	// ----- Setters (check the new value, throw an exception if wrong) -----
	void setProgress(double newProgress);
	void setFinalScore(double newScore);
	void setCertificateIssued(bool issued);

	// One line of the data file: ID|Name|CourseID|Date|Progress|Score|Certificate(0/1)
	string toFileLine() const;
};

#endif
