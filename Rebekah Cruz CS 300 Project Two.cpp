// Rebekah Cruz CS 300 Project Two.cpp
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>

using namespace std;

// Stores each course's number, title, and prerequisites

struct Course {
	string courseNumber;
	string courseTitle;
	vector<string> prerequisites;
};

// Each node holds a course and points to its left and right children

struct Node {
	Course course;
	Node* left;
	Node* right;

	Node(Course newCourse) {
		course = newCourse;
		left = nullptr;
		right = nullptr;
	}
};

void ClearTree(Node* node);

// Adds a course to the tree based on course number

Node* InsertCourse(Node* node, Course course) {
	if (node == nullptr) {
		return new Node(course);
	}

	if (course.courseNumber < node->course.courseNumber) {
		node->left = InsertCourse(node->left, course);
	}
	else if (course.courseNumber > node->course.courseNumber) {
		node->right = InsertCourse(node->right, course);
	}

	return node;
}

// Searches the tree for a course using its course number

Node* SearchCourse(Node* node, string courseNumber) {
	if (node == nullptr) {
		return nullptr;
	}

	if (courseNumber == node->course.courseNumber) {
		return node;
	}

	if (courseNumber < node->course.courseNumber) {
		return SearchCourse(node->left, courseNumber);
	}
	else {
		return SearchCourse(node->right, courseNumber);
	}
}

// Prints courses in order by their course numbers

void PrintCourseList(Node* node) {
	if (node == nullptr) {
		return;
	}

	PrintCourseList(node->left);

	cout << node->course.courseNumber << ", "
		<< node->course.courseTitle << endl;

	PrintCourseList(node->right);
}

// Reads the course file and loads the courses into the tree

bool LoadCourses(string fileName, Node*& root) {
	ifstream file(fileName);

	if (!file.is_open()) {
		cout << "Error: Could not open file." << endl;
		return false;
	}

	vector<Course> courseList;
	string line;

	while (getline(file, line)) {
		stringstream ss(line);
		string item;
		Course course;

		if (!getline(ss, course.courseNumber, ',') ||
			!getline(ss, course.courseTitle, ',') ||
			course.courseNumber.empty() ||
			course.courseTitle.empty()) {
			cout << "Error: Course information missing." << endl;
			return false;
		}

		while (getline(ss, item, ',')) {
			if (!item.empty()) {
				course.prerequisites.push_back(item);
			}
		}

	// Checks if the course number is already in the list
		bool duplicate = false;

		for (const Course& existingCourse : courseList) {
			if (course.courseNumber == existingCourse.courseNumber) {
				duplicate = true;
				break;
			}
		}

		if (duplicate) {
			cout << "Error: Duplicate course number "
				<< course.courseNumber << endl;
			return false;
		}

		courseList.push_back(course);
	}

	file.close();

	if (courseList.empty()) {
		cout << "Error: The course file is empty" << endl;
		return false;
	}

	// Checks that each prerequisite exists in the course list

	for (const Course& course : courseList) {
		for (const string& prerequisite : course.prerequisites) {
			bool found = false;

			for (const Course& otherCourse : courseList) {
				if (prerequisite == otherCourse.courseNumber) {
					found = true;
					break;
				}
			}

			if (!found) {
				cout << "Error: Prerequisite " << prerequisite
					<< " was not found." << endl;
				return false;

			}
		}
	}

	// Loads the new courses into a separate tree first

	Node* newRoot = nullptr;

	for (const Course& course : courseList) {
		newRoot = InsertCourse(newRoot, course);
	}

	// Replaces the old tree with the new one

	ClearTree(root);
	root = newRoot;

	cout << "Course data loaded successfully" << endl;
	return true;
}

// Displays a course and its prerequisites

void PrintCourse(Node* root, string courseNumber) {
	Node* foundCourse = SearchCourse(root, courseNumber);

	if (foundCourse == nullptr) {
		cout << "Course not found" << endl;
		return;
	}

	cout << foundCourse->course.courseNumber << ", "
		<< foundCourse->course.courseTitle << endl;

	if (foundCourse->course.prerequisites.empty()) {
		cout << "Prerequisites: None" << endl;
	}
	else {
		cout << "Prerequisites: " << endl;

		for (const string& prerequisite : foundCourse->course.prerequisites) {
			Node* prereqCourse = SearchCourse(root, prerequisite);

			if (prereqCourse != nullptr) {
				cout << prerequisite << ", "
					<< prereqCourse->course.courseTitle << endl;
			}
		}
	}
}

// Removes the nodes from the tree when they're no longer needed

void ClearTree(Node* node) {
	if (node == nullptr) {
		return;
	}

	ClearTree(node->left);
	ClearTree(node->right);

	delete node;
}

int main() {
	Node* root = nullptr;
	int choice = 0;
	string fileName;
	string courseNumber;

	cout << "Welcome to the course planner" << endl;

	while (choice != 9) {
		cout << endl;
		cout << "1. Load Data Structure" << endl;
		cout << "2. Print Course List" << endl;
		cout << "3. Print Course" << endl;
		cout << "9. Exit" << endl;
		cout << endl;
		cout << "What would you like to do? ";
		

		if (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid option. Please enter a number" << endl;
			continue;
		}

		switch (choice) {
		case 1:
			cout << "Enter the file name: ";
			cin >> ws;
			getline(cin, fileName);
			LoadCourses(fileName, root);
			break;

		case 2:
			if (root == nullptr) {
				cout << "Please load the course data first" << endl;
			}
			else {
				cout << "Here is a sample schedule:" << endl;
				PrintCourseList(root);
			}
			break;

		case 3:
			if (root == nullptr) {
				cout << "Please load the course data first" << endl;
			}
			else {
				cout << "What course do you want to know about? ";
				cin >> courseNumber;

				// Changes lowercase letters to uppercase before searching
				for (char& letter : courseNumber) {
					letter = toupper(static_cast<unsigned char>(letter));
				}

				PrintCourse(root, courseNumber);
			}
			break;

		case 9:
			cout << "Thank you for using the course planner" << endl;
			break;

		default:
			cout << "Invalid option. Please try again" << endl;
			break;
		}
	}

	ClearTree(root);
	return 0;
}
