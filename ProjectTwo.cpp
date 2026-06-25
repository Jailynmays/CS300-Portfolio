// ProjectTwo.cpp
// CS 300 Project Two - Advising Assistance Program
// Jailyn Mays

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Course object used to store course information from the input file.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Removes extra spaces from the beginning and end of a string.
string trim(const string& value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

// Converts a string to uppercase so course lookups are not case sensitive.
string toUpperCase(string value) {
    for (char& character : value) {
        character = static_cast<char>(toupper(static_cast<unsigned char>(character)));
    }
    return value;
}

// Splits one CSV line into separate fields.
vector<string> splitCsvLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream lineStream(line);

    while (getline(lineStream, token, ',')) {
        tokens.push_back(trim(token));
    }

    // Keeps the last blank field when a line ends with a comma.
    if (!line.empty() && line.back() == ',') {
        tokens.push_back("");
    }

    return tokens;
}

// Loads the course file into the hash table.
bool loadCourses(const string& fileName, unordered_map<string, Course>& courses) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: File could not be opened." << endl;
        return false;
    }

    unordered_map<string, Course> loadedCourses;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        lineNumber++;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitCsvLine(line);

        if (tokens.size() < 2 || tokens.at(0).empty() || tokens.at(1).empty()) {
            cout << "Error: Invalid file format on line " << lineNumber << "." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens.at(0));
        course.courseTitle = tokens.at(1);

        for (size_t i = 2; i < tokens.size(); ++i) {
            string prerequisite = toUpperCase(tokens.at(i));
            if (!prerequisite.empty()) {
                course.prerequisites.push_back(prerequisite);
            }
        }

        loadedCourses[course.courseNumber] = course;
    }

    inputFile.close();

    // Checks that every prerequisite listed in the file is also a valid course.
    for (const auto& pair : loadedCourses) {
        const Course& course = pair.second;
        for (const string& prerequisite : course.prerequisites) {
            if (loadedCourses.find(prerequisite) == loadedCourses.end()) {
                cout << "Error: Prerequisite " << prerequisite
                     << " for course " << course.courseNumber
                     << " was not found in the file." << endl;
                return false;
            }
        }
    }

    courses = loadedCourses;
    cout << "Data structure loaded successfully." << endl;
    return true;
}

// Prints all courses in alphanumeric order.
void printCourseList(const unordered_map<string, Course>& courses) {
    vector<string> courseNumbers;

    for (const auto& pair : courses) {
        courseNumbers.push_back(pair.first);
    }

    sort(courseNumbers.begin(), courseNumbers.end());

    cout << "Here is a sample schedule:" << endl;
    for (const string& courseNumber : courseNumbers) {
        const Course& course = courses.at(courseNumber);
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }
}

// Prints one course and its prerequisites.
void printCourseInformation(const unordered_map<string, Course>& courses) {
    string courseNumber;

    cout << "What course do you want to know about? ";
    cin >> courseNumber;
    courseNumber = toUpperCase(courseNumber);

    auto iterator = courses.find(courseNumber);
    if (iterator == courses.end()) {
        cout << "Course not found." << endl;
        return;
    }

    const Course& course = iterator->second;
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            string prerequisiteNumber = course.prerequisites.at(i);
            cout << prerequisiteNumber;

            auto prerequisiteIterator = courses.find(prerequisiteNumber);
            if (prerequisiteIterator != courses.end()) {
                cout << " - " << prerequisiteIterator->second.courseTitle;
            }

            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

// Displays the main menu.
void displayMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

int main() {
    unordered_map<string, Course> courses;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number from the menu." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            string fileName;
            cout << "Enter the course file name: ";
            cin.ignore(10000, '\n');
            getline(cin, fileName);
            dataLoaded = loadCourses(fileName, courses);
        } else if (choice == 2) {
            if (!dataLoaded) {
                cout << "Please load the data structure first." << endl;
            } else {
                printCourseList(courses);
            }
        } else if (choice == 3) {
            if (!dataLoaded) {
                cout << "Please load the data structure first." << endl;
            } else {
                printCourseInformation(courses);
            }
        } else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        } else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
