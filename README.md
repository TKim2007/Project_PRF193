# Online Course Enrollment & Certification

A console program written in C++ that manages online courses, learner enrollments, progress and certificates.
It is the Topic 05 project of course **PRF193** (class **IC2102**, Group 3, lecturer Phạm Minh Trí).

## Features

**Course management**
- Add a Free or Paid course (the ID `CRSxxx` is generated automatically)
- Update title, instructor, duration and tuition fee
- Delete a course (refused while the course still has learners)
- Show all courses, show by category (Free / Paid)
- Search by title or instructor (keyword can be a part of the text, upper/lower case is ignored)
- Sort by tuition fee (ascending)

**Enrollment management**
- Add a learner to a course (the ID `ENRxxx` is generated automatically, a learner cannot enroll in the same course twice)
- Delete a learner (remove an enrollment)
- Show all enrollments, search by learner name, sort by final score (high to low)
- Update progress and final score; the certificate is issued automatically
- Issue certificates for all eligible learners at once

**Report and files**
- Report: number of learners of every course (`map<courseId, count>`)
- Data is loaded from two text files when the program starts and saved when the user chooses *Save* or *Exit*
- Damaged lines in the data files are skipped and counted, the program keeps running

**Safe input**: every value is checked right after it is typed. A wrong value (for example title `-5`, name `Le 123`, date `2024-02-30`, progress `150`) shows an error message and the same question is asked again.

## Certificate rules

| Course type | Certificate is issued when |
|---|---|
| Free | progress = 100 |
| Paid | progress >= 80 **and** final score >= 50 |

Progress and score are in the range 0 - 100. A Free course always has tuition fee = 0.

## Project structure

```
.
├── main.cpp            # main menu and program loop
├── ConsoleIO.h/.cpp    # menus, safe input (retry until valid), printing tables
├── CourseManager.h/.cpp# data (vector, map) and business rules, template search/sort
├── Course.h/.cpp       # Course (abstract), FreeCourse, PaidCourse + validation
├── Enrollment.h/.cpp   # Enrollment + validation (date, progress, score)
├── FileHelper.h/.cpp   # load and save courses.txt / enrollments.txt
├── courses.txt         # sample data
└── enrollments.txt     # sample data
```

## What is used

| Requirement | Where |
|---|---|
| OOP: encapsulation, inheritance, polymorphism | `Course` (abstract) → `FreeCourse`, `PaidCourse`; virtual `isEligibleForCertificate()` |
| STL `vector` and `map` | `vector<shared_ptr<Course>>`, `vector<shared_ptr<Enrollment>>`, `map<string,int> enrollmentCount` |
| Smart pointers | `shared_ptr`, `make_shared` (no `new` / `delete`) |
| Templates | `findFirst`, `searchBy`, `sortBy` work for both Course and Enrollment; the rule is a named function (`isCheaper`, `hasHigherScore`, ...) |
| Exceptions | `invalid_argument` (wrong value), `out_of_range` (not found), `runtime_error` (rule broken) |
| File I/O | `courses.txt`, `enrollments.txt`, fields separated by `\|` |

## Build and run

You need a C++ compiler (g++ 5 or newer, C++11).

```bash
g++ -std=c++11 -Wall *.cpp -o online_course
./online_course          # Windows: online_course.exe
```

Run the program in the folder that contains `courses.txt` and `enrollments.txt`.
If a file does not exist the program starts with an empty list and creates it when saving.

## Menu

```
MAIN MENU
1. Course Management
2. Enrollment Management
3. Show Report
4. Save Data
0. Exit (saves automatically)
```

Course Management: 1 Add, 2 Update, 3 Delete, 4 Show All, 5 Search, 6 Show by Category, 7 Sort by Fee, 0 Back.
Enrollment Management: 1 Add Learner, 2 Delete Learner, 3 Show All, 4 Search by Name, 5 Sort by Score, 6 Update Progress & Score, 7 Issue Certificates, 0 Back.

## Data file format

`courses.txt`: `Type|CourseID|Title|Instructor|Hours|Fee`
```
Free|CRS001|C++ Basics|Nguyen Van A|20|0
Paid|CRS002|Data Structures with Python|Jane Doe|60|120.5
```

`enrollments.txt`: `EnrollmentID|Learner|CourseID|Date|Progress|Score|Certificate(0/1)`
```
ENR001|Pham Van D|CRS001|2026-09-10|100|0|1
ENR003|Pham Van D|CRS002|2026-09-15|60|45|0
```

Note: the data files must use normal line endings of your system. Files saved with Windows (CRLF) endings can make every line invalid on Linux or macOS; convert them with `dos2unix` or `tr -d '\r'`.

## Limitations

- Names and titles accept English letters only (no Vietnamese accents)
- IDs stop at `CRS999` and `ENR999`
- Search finds the keyword inside the text only; bubble sort is slow for very long lists
- Progress can be lowered later, and an issued certificate is never taken back
- Data is stored in plain text files, one user at a time, console only

## Author
| Trần Nhựt Trọng Kim 

