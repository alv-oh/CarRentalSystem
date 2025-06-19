#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>    // For std::isspace
#include <algorithm> // For std::all_of, std::find_if

using namespace std;

// Define structs
struct User {
    string username;
    string password;
    double accountBalance;
    bool hasBorrowRequest;
    string requestedBrand;
    bool hasReturnRequest;
    string borrowedBrand;
};

struct Car {
    string brand;
    int availableCount;
};

struct Admin {
    string username;
    string password;
};

// Function to check if a string is a valid integer
bool isValidInteger(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// File I/O Functions
Admin loadAdmin() {
    Admin admin;
    ifstream file("admin.txt");
    if (file.is_open()) {
        string line;
        getline(file, line);
        stringstream ss(line);
        string token;
        getline(ss, token, ',');
        admin.username = token;
        getline(ss, token, ',');
        admin.password = token;
        file.close();
    } else {
        admin.username = "admin";
        admin.password = "admin123"; // Default admin
    }
    return admin;
}

vector<User> loadUsers() {
    vector<User> users;
    ifstream file("usersDB.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            // Skip empty lines or lines with only whitespace
            if (line.empty() || all_of(line.begin(), line.end(), [](char c) { return isspace(c); })) {
                continue;
            }
            stringstream ss(line);
            string token;
            User user;
            getline(ss, token, ',');
            user.username = token;
            getline(ss, token, ',');
            user.password = token;
            getline(ss, token, ',');
            try {
                user.accountBalance = stod(token);
            } catch (const invalid_argument&) {
                continue; // Skip invalid balance
            }
            getline(ss, token, ',');
            user.hasBorrowRequest = (token == "1");
            getline(ss, token, ',');
            user.requestedBrand = token;
            getline(ss, token, ',');
            user.hasReturnRequest = (token == "1");
            getline(ss, token, ',');
            user.borrowedBrand = token;
            users.push_back(user);
        }
        file.close();
    }
    return users;
}

vector<Car> loadCars() {
    vector<Car> cars;
    ifstream file("carsDB.txt");
    if (!file.is_open()) {
        cout << "Note: carsDB.txt not found. Starting with empty car list." << endl;
        return cars;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        // Skip empty lines or lines with only whitespace
        if (line.empty() || all_of(line.begin(), line.end(), [](char c) { return isspace(c); })) {
            continue;
        }

        stringstream ss(line);
        string brand, countStr;
        if (!getline(ss, brand, ',') || !getline(ss, countStr, ',')) {
            cout << "Warning: Skipping malformed line " << lineNumber 
                 << " in carsDB.txt: " << line << endl;
            continue;
        }

        brand.erase(brand.find_last_not_of(" \t\n\r") + 1);
        brand.erase(0, brand.find_first_not_of(" \t\n\r"));
        countStr.erase(countStr.find_last_not_of(" \t\n\r") + 1);
        countStr.erase(0, countStr.find_first_not_of(" \t\n\r"));

        if (!isValidInteger(countStr)) {
            cout << "Warning: Invalid car count in line " << lineNumber 
                 << " in carsDB.txt: " << countStr << endl;
            continue;
        }

        try {
            Car car;
            car.brand = brand;
            car.availableCount = stoi(countStr);
            if (car.availableCount < 0) {
                cout << "Warning: Negative count in line " << lineNumber 
                     << " ignored: " << line << endl;
                continue;
            }
            cars.push_back(car);
        } catch (const invalid_argument& e) {
            cout << "Error: Invalid number format in line " << lineNumber 
                 << " in carsDB.txt: " << countStr << endl;
            continue;
        } catch (const out_of_range& e) {
            cout << "Error: Number out of range in line " << lineNumber 
                 << " in carsDB.txt: " << countStr << endl;
            continue;
        }
    }

    file.close();
    return cars;
}

void saveAdmin(const Admin& admin) {
    ofstream file("admin.txt");
    if (file.is_open()) {
        file << admin.username << "," << admin.password << endl;
        file.close();
    } else {
        cout << "Error: Could not open admin.txt for writing." << endl;
    }
}

void saveUsers(const vector<User>& users) {
    ofstream file("usersDB.txt");
    if (file.is_open()) {
        for (const auto& user : users) {
            file << user.username << "," << user.password << "," << user.accountBalance << ","
                 << (user.hasBorrowRequest ? "1" : "0") << "," << user.requestedBrand << ","
                 << (user.hasReturnRequest ? "1" : "0") << "," << user.borrowedBrand << endl;
        }
        file.close();
    } else {
        cout << "Error: Could not open usersDB.txt for writing." << endl;
    }
}

void saveCars(const vector<Car>& cars) {
    ofstream file("carsDB.txt");
    if (file.is_open()) {
        for (const auto& car : cars) {
            if (car.availableCount >= 0) {
                file << car.brand << "," << car.availableCount << endl;
            }
        }
        file.close();
    } else {
        cout << "Error: Could not open carsDB.txt for writing." << endl;
    }
}

// Login Functions
bool adminLogin(const Admin& admin) {
    string inputUsername, inputPassword;
    cout << "Enter admin username: ";
    cin >> inputUsername;
    cout << "Enter admin password: ";
    cin >> inputPassword;
    if (inputUsername == admin.username && inputPassword == admin.password) {
        cout << "Admin login successful!\n";
        return true;
    } else {
        cout << "Invalid username or password.\n";
        return false;
    }
}

int userLogin(const vector<User>& users) {
    string inputUsername, inputPassword;
    cout << "Enter username: ";
    cin >> inputUsername;
    cout << "Enter password: ";
    cin >> inputPassword;
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].username == inputUsername && users[i].password == inputPassword) {
            cout << "User login successful!\n";
            return i;
        }
    }
    cout << "Invalid username or password.\n";
    return -1;
}

// Admin Functions
void registerUser(vector<User>& users) {
    User newUser;
    cout << "Enter new username: ";
    cin >> newUser.username;
    for (const auto& user : users) {
        if (user.username == newUser.username) {
            cout << "Username already exists.\n";
            return;
        }
    }
    cout << "Enter new password: ";
    cin >> newUser.password;
    newUser.accountBalance = 10500.0;
    newUser.hasBorrowRequest = false;
    newUser.requestedBrand = "";
    newUser.hasReturnRequest = false;
    newUser.borrowedBrand = "";
    users.push_back(newUser);
    cout << "User registered with 10500 shillings.\n";
}

void approveBorrowRequests(vector<User>& users, vector<Car>& cars) {
    for (auto& user : users) {
        if (user.hasBorrowRequest) {
            cout << "User " << user.username << " requests " << user.requestedBrand << endl;
            cout << "Approve? (y/n): ";
            string approve;
            cin >> approve;
            if (approve == "y") {
                auto it = find_if(cars.begin(), cars.end(), 
                    [&user](Car& c) { return c.brand == user.requestedBrand; });
                if (it != cars.end() && it->availableCount > 0) {
                    it->availableCount--;
                    user.borrowedBrand = user.requestedBrand;
                    user.hasBorrowRequest = false;
                    user.requestedBrand = "";
                    cout << "Borrow request approved.\n";
                } else {
                    cout << "Car not available.\n";
                }
            }
        }
    }
}

void acceptReturnedCars(vector<User>& users, vector<Car>& cars) {
    for (auto& user : users) {
        if (user.hasReturnRequest) {
            cout << "User " << user.username << " returns " << user.borrowedBrand << endl;
            cout << "Accept? (y/n): ";
            string accept;
            cin >> accept;
            if (accept == "y") {
                auto it = find_if(cars.begin(), cars.end(), 
                    [&user](Car& c) { return c.brand == user.borrowedBrand; });
                if (it != cars.end()) {
                    it->availableCount++;
                    user.borrowedBrand = "";
                    user.hasReturnRequest = false;
                    cout << "Return accepted.\n";
                }
            }
        }
    }
}

void changeAdminPassword(Admin& admin) {
    cout << "Enter new password: ";
    cin >> admin.password;
    cout << "Admin password updated.\n";
}

// User Functions
void viewProfile(const User& user) {
    cout << "Username: " << user.username << endl;
    cout << "Balance: " << user.accountBalance << " shillings\n";
}

void requestRentCar(User& user, const vector<Car>& cars) {
    if (user.hasBorrowRequest || !user.borrowedBrand.empty()) {
        cout << "You have a pending request or a car already.\n";
        return;
    }
    if (user.accountBalance < 5000) {
        cout << "Need at least 5000 shillings to rent.\n";
        return;
    }
    cout << "Available cars:\n";
    for (const auto& car : cars) {
        cout << car.brand << ": " << car.availableCount << endl;
    }
    cout << "Enter brand to rent: ";
    string brand;
    cin >> brand;
    auto it = find_if(cars.begin(), cars.end(), 
        [&brand](const Car& c) { return c.brand == brand && c.availableCount > 0; });
    if (it != cars.end()) {
        user.hasBorrowRequest = true;
        user.requestedBrand = brand;
        cout << "Request submitted for " << brand << ".\n";
    } else {
        cout << "Car not available.\n";
    }
}

void requestReturnCar(User& user) {
    if (user.borrowedBrand.empty()) {
        cout << "No car to return.\n";
        return;
    }
    user.hasReturnRequest = true;
    cout << "Return request submitted.\n";
}

void changeUserPassword(User& user) {
    cout << "Enter new password: ";
    cin >> user.password;
    cout << "Password updated.\n";
}

// Main Program
int main() {
    Admin admin = loadAdmin();
    vector<User> users = loadUsers();
    vector<Car> cars = loadCars();

    bool running = true;
    while (running) {
        cout << "\n=== Car Rental System ===\n";
        cout << "1. Admin Login\n2. User Login\n3. Exit\n";
        cout << "Choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            if (adminLogin(admin)) {
                bool adminLoggedIn = true;
                while (adminLoggedIn) {
                    cout << "\n=== Admin Menu ===\n";
                    cout << "1. Register User\n2. Approve Borrow Requests\n3. Accept Returned Cars\n";
                    cout << "4. Change Password\n5. Logout\n";
                    cout << "Choice: ";
                    int adminChoice;
                    cin >> adminChoice;
                    switch (adminChoice) {
                        case 1: registerUser(users); break;
                        case 2: approveBorrowRequests(users, cars); break;
                        case 3: acceptReturnedCars(users, cars); break;
                        case 4: changeAdminPassword(admin); break;
                        case 5: adminLoggedIn = false; cout << "Logged out.\n"; break;
                        default: cout << "Invalid choice.\n";
                    }
                }
            }
        } else if (choice == 2) {
            int userIndex = userLogin(users);
            if (userIndex != -1) {
                bool userLoggedIn = true;
                while (userLoggedIn) {
                    cout << "\n=== User Menu ===\n";
                    cout << "1. View Profile\n2. Request to Rent Car\n3. Request to Return Car\n";
                    cout << "4. Change Password\n5. Logout\n";
                    cout << "Choice: ";
                    int userChoice;
                    cin >> userChoice;
                    switch (userChoice) {
                        case 1: viewProfile(users[userIndex]); break;
                        case 2: requestRentCar(users[userIndex], cars); break;
                        case 3: requestReturnCar(users[userIndex]); break;
                        case 4: changeUserPassword(users[userIndex]); break;
                        case 5: userLoggedIn = false; cout << "Logged out.\n"; break;
                        default: cout << "Invalid choice.\n";
                    }
                }
            }
        } else if (choice == 3) {
            running = false;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    saveAdmin(admin);
    saveUsers(users);
    saveCars(cars);
    cout << "Data saved. Exiting...\n";
    return 0;
}