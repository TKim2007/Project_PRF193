#include "ConsoleIO.h"
#include "FileHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cctype>
using namespace std;

//==================================================================
// 1 - MENUS
//==================================================================
void io_showMainMenu(){
	cout
	<< "\n============================================\n"
	<< "   ONLINE COURSE MANAGEMENT SYSTEM\n"
	<< "============================================\n"
	<< "1. Course Management\n"
	<< "2. Enrollment Management\n"
	<< "3. Report: Number of learners per course\n"
	<< "4. Save Data to File\n"
	<< "0. Save & Exit\n"
	<< "--------------------------------------------\n";
}

void io_showCourseMenu(){
	cout
	<< "\n------------ COURSE MANAGEMENT ------------\n"
	<< "1. Add New Course\n"
	<< "2. Update Course (details & tuition fee)\n"
	<< "3. Delete Course\n"
	<< "4. Show All Courses\n"
	<< "5. Search Course by Title / Instructor\n"
	<< "6. Show Courses by Category (Free / Paid)\n"
	<< "7. Sort Courses by Tuition Fee (Ascending)\n"
	<< "0. Back\n"
	<< "--------------------------------------------\n";
}

void io_showEnrollmentMenu(){
	cout
	<< "\n----------- ENROLLMENT MANAGEMENT -----------\n"
	<< "1. Add Learner (Enroll into a Course)\n"
	<< "2. Delete Learner (Remove an Enrollment)\n"
	<< "3. Show All Enrollments\n"
	<< "4. Search Enrollment by Learner Name\n"
	<< "5. Sort Enrollments by Final Score (High -> Low)\n"
	<< "6. Update Progress & Final Score (auto certificate)\n"
	<< "7. Issue Certificates (all eligible learners)\n"
	<< "0. Back\n"
	<< "--------------------------------------------\n";
}

// 2 - SAFE INPUT
// Removes the spaces at the beginning and at the end of a text.
static string trim(const string &text){
	int start = 0;
	int end = text.length() - 1;
	while(start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\n' || text[start] == '\r')){
		start++;
	}
	while(end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\n' || text[end] == '\r')){
		end--;
	}
	if(start > end){
		return "";
	}
	return text.substr(start, end - start + 1);
}

// Makes a copy of the text in upper case (the user can type "crs001").
static string toUpper(const string &text){
	string result = text;
	int length = result.length();
	for(int i = 0; i < length; i++){
		result[i] = (char)toupper((unsigned char)result[i]);
	}
	return result;
}

// Prints the prompt and reads one whole line.
static string readLine(const string &prompt){
	cout << prompt;
	string line;
	if(!getline(cin, line)){	// the input is closed: stop the program
		throw InputClosed();
	}
	return trim(line);
}

// Turns a text into an integer. "12abc" and "3.5" are refused.
static int parseInteger(const string &text){
	istringstream in(text);
	int value;
	in >> value;
	if(in.fail()){
		throw invalid_argument("Please enter a whole number");
	}
	char extra;
	if(in >> extra){
		throw invalid_argument("Please enter a whole number (no extra characters)");
	}
	return value;
}

// Turns a number into text without useless zeros: 120.5 -> "120.5", 60 -> "60"
static string numberToText(double value){
	ostringstream out;
	out << setprecision(10) << value;
	return out.str();
}

// Turns a number into text with a fixed number of decimals: (60, 1) -> "60.0"
static string fixedText(double value, int decimals){
	ostringstream out;
	out << fixed << setprecision(decimals) << value;
	return out.str();
}

// ----- text fields -----
string io_readCourseTitle(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid course title
	do{
		text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			validateTitle(text);	// throws invalid_argument when the value is wrong
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

string io_readInstructorName(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid instructor name
	do{
		text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			validateInstructorName(text);	// throws invalid_argument when the value is wrong
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

string io_readLearnerName(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid learner name
	do{
		text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			validateLearnerName(text);	// throws invalid_argument when the value is wrong
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

string io_readDate(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid date
	do{
		text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			validateDate(text);	// throws invalid_argument when the value is wrong
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

string io_readKeyword(const string &prompt){
	string keyword;
	bool valid = false;
	// Loop until the user enters a valid keyword
	do{
		keyword = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			if(keyword == ""){
				throw invalid_argument("Keyword cannot be empty");
			}
			if(keyword.length() > 50){
				throw invalid_argument("Keyword cannot be longer than 50 characters");
			}
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return keyword;
}

// ----- number fields -----
double io_readDuration(const string &prompt){
	double value = 0;
	bool valid = false;
	// Loop until the user enters a valid duration
	do{
		string text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Duration");	// throws if the text is not a number
			validateDuration(value);	// throws if the number is out of the allowed range
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return value;
}

double io_readTuitionFee(const string &prompt){
	double value = 0;
	bool valid = false;
	// Loop until the user enters a valid tuition fee
	do{
		string text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Tuition fee");	// throws if the text is not a number
			validateTuitionFee(value);	// throws if the number is out of the allowed range
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return value;
}

double io_readProgress(const string &prompt){
	double value = 0;
	bool valid = false;
	// Loop until the user enters a valid progress
	do{
		string text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Progress");	// throws if the text is not a number
			validateProgress(value);	// throws if the number is out of the allowed range
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return value;
}

double io_readFinalScore(const string &prompt){
	double value = 0;
	bool valid = false;
	// Loop until the user enters a valid final score
	do{
		string text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Final score");	// throws if the text is not a number
			validateFinalScore(value);	// throws if the number is out of the allowed range
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return value;
}

// ----- optional fields (the update screen) -----
string io_readOptionalTitle(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid course title (or nothing)
	do{
		text = readLine(prompt);
		if(text == ""){
			return "";	// empty = keep the old value
		}
		// Catch standard exceptions and ask again
		try{
			validateTitle(text);
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

string io_readOptionalInstructor(const string &prompt){
	string text;
	bool valid = false;
	// Loop until the user enters a valid instructor name (or nothing)
	do{
		text = readLine(prompt);
		if(text == ""){
			return "";	// empty = keep the old value
		}
		// Catch standard exceptions and ask again
		try{
			validateInstructorName(text);
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return text;
}

bool io_readOptionalDuration(const string &prompt, double &value){
	bool valid = false;
	// Loop until the user enters a valid duration (or nothing)
	do{
		string text = readLine(prompt);
		if(text == ""){
			return false;	// empty = keep the old value
		}
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Duration");
			validateDuration(value);
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return true;
}

bool io_readOptionalTuitionFee(const string &prompt, double &value){
	bool valid = false;
	// Loop until the user enters a valid tuition fee (or nothing)
	do{
		string text = readLine(prompt);
		if(text == ""){
			return false;	// empty = keep the old value
		}
		// Catch standard exceptions and ask again
		try{
			value = parseNumber(text, "Tuition fee");
			validateTuitionFee(value);
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again.\n";
		}
	}while(!valid);
	return true;
}

// ----- other input -----
int io_readChoice(const string &prompt, int minValue, int maxValue){
	int value = 0;
	bool valid = false;
	// Loop until the user enters a whole number in the range
	do{
		string text = readLine(prompt);
		// Catch standard exceptions and ask again
		try{
			value = parseInteger(text);
			if(value < minValue || value > maxValue){
				throw out_of_range("Please choose a number from " + to_string(minValue) + " to " + to_string(maxValue));
			}
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ".\n";
		}
	}while(!valid);
	return value;
}

bool io_confirm(const string &question){
	string answer;
	// Loop until the user answers yes or no
	do{
		answer = toUpper(readLine(question + " (y/n): "));
		if(answer == "Y" || answer == "YES"){
			return true;
		}
		if(answer == "N" || answer == "NO"){
			return false;
		}
		cout << "(!) Error: please answer y or n.\n";
	}while(true);
}

string io_readCourseId(const CourseManager &manager, const string &prompt){
	string id;
	bool valid = false;
	// Loop until the user enters an existing ID (or 0 to cancel)
	do{
		id = toUpper(readLine(prompt));
		if(id == "0"){
			return "";	// the user cancels
		}
		// Catch standard exceptions and ask again
		try{
			validateCourseId(id);	// the format must be CRSxxx
			if(!manager.findCourse(id)){
				throw out_of_range("Course not found: " + id);
			}
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again (0 = cancel).\n";
		}
	}while(!valid);
	return id;
}

string io_readEnrollmentId(const CourseManager &manager, const string &prompt){
	string id;
	bool valid = false;
	// Loop until the user enters an existing ID (or 0 to cancel)
	do{
		id = toUpper(readLine(prompt));
		if(id == "0"){
			return "";
		}
		// Catch standard exceptions and ask again
		try{
			validateEnrollmentId(id);	// the format must be ENRxxx
			if(!manager.findEnrollment(id)){
				throw out_of_range("Enrollment not found: " + id);
			}
			valid = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << ". Please try again (0 = cancel).\n";
		}
	}while(!valid);
	return id;
}

// 3 - DISPLAY
// Cuts a long text so that it never touches the next column.
static string cutText(const string &text, int width){
	if((int)text.length() <= width){
		return text;
	}
	return text.substr(0, width - 2) + "..";
}

void io_displayCourses(const vector<shared_ptr<Course> > &courses, const string &title){
	cout << "\n--- " << title << " ---\n";
	if(courses.empty()){
		cout << "(!) List is empty or no results found.\n";
		return;
	}
	cout << left << setw(8) << "ID" << setw(26) << "Title" << setw(18) << "Instructor"
		<< setw(8) << "Hours" << setw(12) << "Fee" << "Type" << "\n";
	cout << string(78, '-') << "\n";

	int count = courses.size();
	for(int i = 0; i < count; i++){
		shared_ptr<Course> c = courses[i];
		cout << left << setw(8) << c->getCourseId()
			<< setw(26) << cutText(c->getTitle(), 24)
			<< setw(18) << cutText(c->getInstructor(), 16)
			<< setw(8) << fixedText(c->getDurationHours(), 1)
			<< setw(12) << fixedText(c->getTuitionFee(), 2)
			<< c->getType() << "\n";
	}
}

void io_displayEnrollments(const vector<shared_ptr<Enrollment> > &enrollments, const string &title){
	cout << "\n--- " << title << " ---\n";
	if(enrollments.empty()){
		cout << "(!) List is empty or no results found.\n";
		return;
	}
	cout << left << setw(8) << "ID" << setw(22) << "Learner" << setw(9) << "Course" << setw(12) << "Date"
		<< setw(10) << "Progress" << setw(8) << "Score" << "Certificate" << "\n";
	cout << string(78, '-') << "\n";

	int count = enrollments.size();
	for(int i = 0; i < count; i++){
		shared_ptr<Enrollment> e = enrollments[i];
		string certificate = "-";
		if(e->isCertificateIssued()){
			certificate = "Issued";
		}
		cout << left << setw(8) << e->getEnrollmentId()
			<< setw(22) << cutText(e->getLearnerName(), 20)
			<< setw(9) << e->getCourseId()
			<< setw(12) << e->getEnrollmentDate()
			<< setw(10) << fixedText(e->getProgress(), 1)
			<< setw(8) << fixedText(e->getFinalScore(), 1)
			<< certificate << "\n";
	}
}

// 4 - START-UP AND EXIT MESSAGES
void io_showLoadResult(const CourseManager &manager, int skippedLines){
	cout << "(*) Loaded " << manager.getCourses().size() << " course(s) and " << manager.getEnrollments().size() << " enrollment(s).\n";
	if(skippedLines > 0){
		cout << "(!) Warning: " << skippedLines << " invalid line(s) in the data files were skipped.\n";
	}
}

void io_saveData(const CourseManager &manager){
	if(file_save(manager)){
		cout << "(*) Data saved successfully.\n";
	}else{
		cout << "(!) Error: Failed to save data.\n";
	}
}

// 5 - COURSE SCREENS (one function for each menu item)
// ----- 1. Add New Course -----
static void addCourse(CourseManager &manager){
	cout << "\n--- Add New Course ---\n";

	// Every value is checked when it is typed: a wrong value is refused and asked again.
	string title = io_readCourseTitle("Course title: ");
	string instructor = io_readInstructorName("Instructor name: ");
	double hours = io_readDuration("Duration (hours): ");
	int type = io_readChoice("Type (1 = Free, 2 = Paid): ", 1, 2);

	double fee = 0;	// a Free course has no fee
	if(type == 2){
		fee = io_readTuitionFee("Tuition fee: ");
	}

	// Try to create the course; catch the error if the ID cannot be generated or the course is refused
	try{
		string id = manager.generateCourseId();
		shared_ptr<Course> course;
		if(type == 1){
			course = make_shared<FreeCourse>(id, title, instructor, hours);
		}else{
			course = make_shared<PaidCourse>(id, title, instructor, hours, fee);
		}
		manager.addCourse(course);
		cout << "(*) Course " << id << " added successfully.\n";
	}catch(const exception &ex){
		cout << "(!) Error: " << ex.what() << "\n";
	}
}

// ----- 2. Update Course -----
static void updateCourse(CourseManager &manager){
	cout << "\n--- Update Course ---\n";
	io_displayCourses(manager.getCourses(), "Current Courses");
	if(manager.getCourses().empty()){
		return;
	}

	string id = io_readCourseId(manager, "Course ID to update (0 = cancel): ");
	if(id == ""){
		cout << "(*) Update cancelled.\n";
		return;
	}
	shared_ptr<Course> course = manager.findCourse(id);

	// An empty answer means "keep the old value".
	cout << "Leave a field empty to keep its current value.\n";
	string newTitle = io_readOptionalTitle("New title [" + course->getTitle() + "]: ");
	string newInstructor = io_readOptionalInstructor("New instructor [" + course->getInstructor() + "]: ");

	double newHours = 0;
	bool changeHours = io_readOptionalDuration("New duration in hours [" + numberToText(course->getDurationHours()) + "]: ", newHours);

	double newFee = 0;
	bool changeFee = false;
	if(course->getType() == "Paid"){
		changeFee = io_readOptionalTuitionFee("New tuition fee [" + numberToText(course->getTuitionFee()) + "]: ", newFee);
	}else{
		cout << "(i) This is a Free course: the tuition fee stays 0.\n";
	}

	// Try to apply the changes; the setters check the values again
	try{
		if(newTitle != ""){
			course->setTitle(newTitle);
		}
		if(newInstructor != ""){
			course->setInstructor(newInstructor);
		}
		if(changeHours){
			course->setDurationHours(newHours);
		}
		if(changeFee){
			course->setTuitionFee(newFee);
		}
		cout << "(*) Course updated successfully.\n";
	}catch(const exception &ex){
		cout << "(!) Error: " << ex.what() << "\n";
	}
}

// ----- 3. Delete Course -----
static void deleteCourse(CourseManager &manager){
	cout << "\n--- Delete Course ---\n";
	io_displayCourses(manager.getCourses(), "Current Courses");
	if(manager.getCourses().empty()){
		return;
	}

	string id = io_readCourseId(manager, "Course ID to delete (0 = cancel): ");
	if(id == ""){
		cout << "(*) Delete cancelled.\n";
		return;
	}
	if(!io_confirm("Delete course " + id + "?")){
		cout << "(*) Delete cancelled.\n";
		return;
	}

	// Try to delete; catch the error if the course still has learners
	try{
		manager.deleteCourse(id);	// refused when the course still has learners
		cout << "(*) Course deleted successfully.\n";
	}catch(const exception &ex){
		cout << "(!) Error: " << ex.what() << "\n";
	}
}

// ----- 5. Search Course -----
static void searchCourse(CourseManager &manager){
	cout << "\n--- Search Course ---\n";
	string keyword = io_readKeyword("Enter title or instructor keyword: ");
	io_displayCourses(manager.searchCourses(keyword), "Search Results");
}

// ----- 6. Show by category -----
static void showCoursesByCategory(CourseManager &manager){
	io_displayCourses(manager.getCoursesByType("Free"), "FREE COURSES");
	io_displayCourses(manager.getCoursesByType("Paid"), "PAID COURSES");
}

void io_courseMenu(CourseManager &manager){
	int choice;
	// Loop until the user chooses 0 (Back)
	do{
		io_showCourseMenu();
		choice = io_readChoice("Enter your choice: ", 0, 7);
		switch(choice){
			case 1:
				addCourse(manager);
				break;
			case 2:
				updateCourse(manager);
				break;
			case 3:
				deleteCourse(manager);
				break;
			case 4:
				io_displayCourses(manager.getCourses(), "List of All Courses");
				break;
			case 5:
				searchCourse(manager);
				break;
			case 6:
				showCoursesByCategory(manager);
				break;
			case 7:
				io_displayCourses(manager.getCoursesSortedByFee(), "Courses Sorted by Fee (Ascending)");
				break;
			case 0:
				break;
		}
	}while(choice != 0);
}

// 6 - ENROLLMENT SCREENS (one function for each menu item)
// ----- 1. Add Learner -----
static void addLearner(CourseManager &manager){
	cout << "\n--- Add Learner (Enroll) ---\n";
	if(manager.getCourses().empty()){
		cout << "(!) There is no course yet. Please add a course first.\n";
		return;
	}
	io_displayCourses(manager.getCourses(), "Available Courses");

	bool finished = false;
	// Loop until the learner is enrolled or the user stops
	do{
		string name = io_readLearnerName("Learner name: ");
		string courseId = io_readCourseId(manager, "Course ID (0 = cancel): ");
		if(courseId == ""){
			cout << "(*) Enrollment cancelled.\n";
			return;
		}
		string date = io_readDate("Enrollment date (YYYY-MM-DD): ");

		// Try to enroll; catch the error (for example: the learner is already in this course)
		try{
			shared_ptr<Enrollment> enrollment = manager.enrollLearner(name, courseId, date);
			cout << "(*) Enrolled successfully. Enrollment ID: " << enrollment->getEnrollmentId() << "\n";
			finished = true;
		}catch(const exception &ex){
			cout << "(!) Error: " << ex.what() << "\n";
			if(!io_confirm("Do you want to enter the information again?")){
				finished = true;
			}
		}
	}while(!finished);
}

// ----- 2. Delete Learner -----
static void deleteLearner(CourseManager &manager){
	cout << "\n--- Delete Learner (Remove Enrollment) ---\n";
	io_displayEnrollments(manager.getEnrollments(), "Current Enrollments");
	if(manager.getEnrollments().empty()){
		return;
	}

	string id = io_readEnrollmentId(manager, "Enrollment ID to remove (0 = cancel): ");
	if(id == ""){
		cout << "(*) Delete cancelled.\n";
		return;
	}
	if(!io_confirm("Remove enrollment " + id + "?")){
		cout << "(*) Delete cancelled.\n";
		return;
	}

	// Try to delete the enrollment
	try{
		manager.deleteEnrollment(id);
		cout << "(*) Enrollment removed successfully.\n";
	}catch(const exception &ex){
		cout << "(!) Error: " << ex.what() << "\n";
	}
}

// ----- 4. Search Enrollment -----
static void searchEnrollment(CourseManager &manager){
	cout << "\n--- Search Enrollment ---\n";
	string keyword = io_readKeyword("Enter learner name keyword: ");
	io_displayEnrollments(manager.searchEnrollments(keyword), "Search Results");
}

// ----- 6. Update Progress & Final Score -----
static void updateProgress(CourseManager &manager){
	cout << "\n--- Update Progress & Final Score ---\n";
	io_displayEnrollments(manager.getEnrollments(), "Current Enrollments");
	if(manager.getEnrollments().empty()){
		return;
	}

	string id = io_readEnrollmentId(manager, "Enrollment ID (0 = cancel): ");
	if(id == ""){
		cout << "(*) Update cancelled.\n";
		return;
	}

	double progress = io_readProgress("Progress (0 - 100): ");
	double score = io_readFinalScore("Final score (0 - 100): ");

	// Try to update; the certificate is issued automatically when the rule is met
	try{
		bool certificateIssued = manager.updateProgress(id, progress, score);
		cout << "(*) Updated successfully.\n";
		if(certificateIssued){
			cout << "(*) Congratulations! Certificate issued.\n";
		}else{
			cout << "(i) No new certificate: the rule of the course is not met yet (or it was issued before).\n";
		}
	}catch(const exception &ex){
		cout << "(!) Error: " << ex.what() << "\n";
	}
}

// ----- 7. Issue Certificates -----
static void issueCertificates(CourseManager &manager){
	cout << "\n--- Issue Certificates ---\n";
	const vector<shared_ptr<Enrollment> > &list = manager.getEnrollments();
	if(list.empty()){
		cout << "(!) There is no enrollment yet.\n";
		return;
	}

	int issued = 0;
	int count = list.size();
	for(int i = 0; i < count; i++){
		shared_ptr<Enrollment> e = list[i];
		if(e->isCertificateIssued()){
			continue;	// already has a certificate
		}
		shared_ptr<Course> course = manager.findCourse(e->getCourseId());
		if(!course){
			continue;
		}
		if(course->isEligibleForCertificate(e->getProgress(), e->getFinalScore())){
			e->setCertificateIssued(true);
			issued++;
			cout << "(*) Certificate issued: " << e->getEnrollmentId() << " - " << e->getLearnerName()
				<< " (" << course->getTitle() << ", " << course->getType() << " course)\n";
		}
	}

	if(issued == 0){
		cout << "(i) No learner reaches the passing rule yet (Free: progress = 100; Paid: progress >= 80 and score >= 50).\n";
	}else{
		cout << "(*) Total new certificates: " << issued << "\n";
	}
}

void io_enrollmentMenu(CourseManager &manager){
	int choice;
	// Loop until the user chooses 0 (Back)
	do{
		io_showEnrollmentMenu();
		choice = io_readChoice("Enter your choice: ", 0, 7);
		switch(choice){
			case 1:
				addLearner(manager);
				break;
			case 2:
				deleteLearner(manager);
				break;
			case 3:
				io_displayEnrollments(manager.getEnrollments(), "List of All Enrollments");
				break;
			case 4:
				searchEnrollment(manager);
				break;
			case 5:
				io_displayEnrollments(manager.getEnrollmentsSortedByScore(), "Enrollments Sorted by Score (High -> Low)");
				break;
			case 6:
				updateProgress(manager);
				break;
			case 7:
				issueCertificates(manager);
				break;
			case 0:
				break;
		}
	}while(choice != 0);
}


//7 - REPORT (uses the STL map)
void io_showReport(const CourseManager &manager){
	cout << "\n--- Number of Learners per Course ---\n";
	const map<string, int> &counts = manager.getEnrollmentCount();
	if(counts.empty()){
		cout << "(!) No course yet.\n";
		return;
	}

	// it->first is the course ID (the key), it->second is the number of learners (the value)
	int total = 0;
	map<string, int>::const_iterator it;
	for(it = counts.begin(); it != counts.end(); ++it){
		shared_ptr<Course> course = manager.findCourse(it->first);
		if(course){
			cout << it->first << " - " << course->getTitle() << " : " << it->second << " learner(s)\n";
			total += it->second;
		}
	}
	cout << "Total enrollments: " << total << "\n";
}
