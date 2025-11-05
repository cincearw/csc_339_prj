#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

// -----------------------------------------------------
// Student Class
// -----------------------------------------------------
class Student {
protected:
    char name[21];
    char ssn[10];
    float gpa;
    int credits;

public:
    Student() {
        strcpy(name, "");
        strcpy(ssn, "");
        gpa = 0.0;
        credits = 0;
    }

    Student(const char* n, const char* s, int c, float g) {
        strncpy(name, n, 20);
        name[20] = '\0';
        strncpy(ssn, s, 9);
        ssn[9] = '\0';
        credits = c;
        gpa = g;
    }

    virtual void print() const {
        cout << "Student name: " << name << endl;
        cout << "SSN: " << ssn << endl;
        cout << "Credits: " << credits << endl;
        cout << "GPA: " << gpa << endl;
    }

    // pure virtual tuition (must be overridden)
    virtual float tuition() const = 0;

    // for sorting by GPA
    bool operator<(const Student& other) const {
        return gpa < other.gpa;
    }

    virtual ~Student() {}
};

// -----------------------------------------------------
// Undergrad Class
// -----------------------------------------------------
class Undergrad : public Student {
private:
    static float undergrad_rate;
    char* year;

public:
    Undergrad() : Student() {
        year = nullptr;
    }

    Undergrad(const char* n, const char* s, const char* y, int c, float g)
        : Student(n, s, c, g) {
        year = new char[strlen(y) + 1];
        strcpy(year, y);
    }

    char* get_year() { return year; }
    void set_year(const char* y) {
        delete[] year;
        year = new char[strlen(y) + 1];
        strcpy(year, y);
    }

    float tuition() const override {
        return credits * undergrad_rate;
    }

    void print() const override {
        cout << left << setw(20) << name
             << setw(10) << ssn
             << setw(10) << year
             << setw(10) << credits
             << right << setw(10) << fixed << setprecision(2)
             << "$" << tuition()
             << setw(10) << gpa << endl;
    }

    ~Undergrad() {
        delete[] year;
    }
};
float Undergrad::undergrad_rate = 380.0;

// -----------------------------------------------------
// Grad Class
// -----------------------------------------------------
class Grad : public Student {
protected:
    static float grad_rate;
    char* thesis;

public:
    Grad() : Student() {
        thesis = nullptr;
    }

    Grad(const char* n, const char* s, const char* t, int c, float g)
        : Student(n, s, c, g) {
        thesis = new char[strlen(t) + 1];
        strcpy(thesis, t);
    }

    char* get_thesis() { return thesis; }
    void set_thesis(const char* t) {
        delete[] thesis;
        thesis = new char[strlen(t) + 1];
        strcpy(thesis, t);
    }

    float tuition() const override {
        return credits * grad_rate;
    }

    void print() const override {
        cout << left << setw(20) << name
             << setw(10) << ssn
             << setw(10) << "Grad"
             << setw(10) << credits
             << right << setw(10) << fixed << setprecision(2)
             << "$" << tuition()
             << setw(10) << gpa << endl;
    }

    ~Grad() {
        delete[] thesis;
    }
};
float Grad::grad_rate = 500.0;

// -----------------------------------------------------
// GradAsst Class
// -----------------------------------------------------
class GradAsst : public Grad {
private:
    float hourPay;
    char* superviser;
    char* task;

public:
    GradAsst() : Grad() {
        hourPay = 0.0;
        superviser = nullptr;
        task = nullptr;
    }

    GradAsst(const char* n, const char* s, const char* t, int c, float g,
             const char* tk, const char* sv, float hp)
        : Grad(n, s, t, c, g) {
        task = new char[strlen(tk) + 1];
        strcpy(task, tk);
        superviser = new char[strlen(sv) + 1];
        strcpy(superviser, sv);
        hourPay = hp;
    }

    char* get_superviser() { return superviser; }
    float get_hourPay() { return hourPay; }
    char* get_task() { return task; }

    void set_superviser(const char* s) {
        delete[] superviser;
        superviser = new char[strlen(s) + 1];
        strcpy(superviser, s);
    }

    void set_hourPay(float h) { hourPay = h; }
    void set_task(const char* t) {
        delete[] task;
        task = new char[strlen(t) + 1];
        strcpy(task, t);
    }

    float tuition() const override {
        return credits * grad_rate;  // same as Grad
    }

    void print() const override {
        cout << left << setw(20) << name
             << setw(10) << ssn
             << setw(10) << "GA"
             << setw(10) << credits
             << right << setw(10) << fixed << setprecision(2)
             << "$" << tuition()
             << setw(10) << gpa << endl;
    }

    ~GradAsst() {
        delete[] superviser;
        delete[] task;
    }
};

// -----------------------------------------------------
// print header
// -----------------------------------------------------
void printHeader() {
    cout << left << setw(20) << "Name"
         << setw(10) << "SSN"
         << setw(10) << "Year"
         << setw(10) << "Credits"
         << right << setw(10) << "Tuition"
         << setw(10) << "GPA" << endl;
    cout << string(70, '-') << endl;
}

// -----------------------------------------------------
// Main
// -----------------------------------------------------
int main() {
    ifstream fin("students.txt");
    if (!fin) {
        cerr << "Error: cannot open students.txt\n";
        return 1;
    }

    list<Student*> students;
    string type;

    // Read each record and construct appropriate object
    while (fin >> type) {
        if (type == "Undergrad") {
            string name, ssn, year;
            int credits;
            float gpa;
            fin >> name >> ssn >> year >> credits >> gpa;
            students.push_back(new Undergrad(name.c_str(), ssn.c_str(), year.c_str(), credits, gpa));
        } else if (type == "Grad") {
            string name, ssn, degree;
            int credits;
            float gpa;
            string thesis;
            fin >> name >> ssn >> degree >> credits >> gpa;
            getline(fin, thesis);
            if (!thesis.empty() && thesis[0] == ' ') thesis.erase(0, 1);
            students.push_back(new Grad(name.c_str(), ssn.c_str(), thesis.c_str(), credits, gpa));
        } else if (type == "GradAsst") {
            string name, ssn, degree;
            int credits;
            float gpa;
            string thesis, task, supervisor;
            float hourPay;
            fin >> name >> ssn >> degree >> credits >> gpa;
            getline(fin, thesis, '"'); // skip until first quote
            getline(fin, thesis, '"'); // read thesis inside quotes
            getline(fin, task, '"');   // skip until next quote
            getline(fin, task, '"');
            fin >> ws;
            getline(fin, supervisor, '"');
            getline(fin, supervisor, '"');
            fin >> hourPay;
            students.push_back(new GradAsst(name.c_str(), ssn.c_str(), thesis.c_str(),
                                            credits, gpa, task.c_str(), supervisor.c_str(), hourPay));
        }
    }
    fin.close();

    // Move last student to front
    if (!students.empty()) {
        Student* last = students.back();
        students.pop_back();
        students.push_front(last);
    }

    // Display all students
    cout << "\nOriginal Student List:\n";
    printHeader();
    for (auto s : students) s->print();

    // Copy list to vector
    vector<Student*> vec(students.begin(), students.end());

    // Sort by GPA 
    sort(vec.begin(), vec.end(),
         [](Student* a, Student* b) { return *a < *b; });

    cout << "\nStudents Sorted by GPA:\n";
    printHeader();
    for (auto s : vec) s->print();

    // Cleanup dynamic memory
    for (auto s : students) delete s;
    students.clear();

    return 0;
}