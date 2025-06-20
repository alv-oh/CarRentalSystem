#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Simple data structures
struct User {
    string username;
    string password;
    double balance;
    bool wantsToBorrow;
    string wantedCar;
    bool wantsToReturn;
    string borrowedCar;
};

struct Car {
    string name;
    int count;
};

struct Admin {
    string username;
    string password;
};

// Global variables to store data
Admin admin;
vector<User> users;
vector<Car> cars;

// Load data from files
void loadData() {
    // Load admin
    admin.username = "admin";
    admin.password = "admin123";
    
    ifstream adminFile("admin.txt");
    if (adminFile.is_open()) {
        getline(adminFile, admin.username, ',');
        getline(adminFile, admin.password);
        adminFile.close();
    }
    
    // Load users
    users.clear();
    ifstream userFile("usersDB.txt");
    if (userFile.is_open()) {
        string line;
        while (getline(userFile, line)) {
            if (line.empty()) continue;
            
            User user;
            size_t pos = 0;
            
            // Get username
            pos = line.find(',');
            user.username = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            // Get password
            pos = line.find(',');
            user.password = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            // Get balance
            pos = line.find(',');
            user.balance = stod(line.substr(0, pos));
            line = line.substr(pos + 1);
            
            // Get borrow request
            pos = line.find(',');
            user.wantsToBorrow = (line.substr(0, pos) == "1");
            line = line.substr(pos + 1);
            
            // Get wanted car
            pos = line.find(',');
            user.wantedCar = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            // Get return request
            pos = line.find(',');
            user.wantsToReturn = (line.substr(0, pos) == "1");
            line = line.substr(pos + 1);
            
            // Get borrowed car
            user.borrowedCar = line;
            
            users.push_back(user);
        }
        userFile.close();
    }
    
    // Load cars
    cars.clear();
    ifstream carFile("carsDB.txt");
    if (carFile.is_open()) {
        string line;
        while (getline(carFile, line)) {
            if (line.empty()) continue;
            
            Car car;
            size_t pos = line.find(',');
            car.name = line.substr(0, pos);
            car.count = stoi(line.substr(pos + 1));
            
            cars.push_back(car);
        }
        carFile.close();
    }
}

// Save data to files
void saveData() {
    // Save admin
    ofstream adminFile("admin.txt");
    if (adminFile.is_open()) {
        adminFile << admin.username << "," << admin.password << endl;
        adminFile.close();
    }
    
    // Save users
    ofstream userFile("usersDB.txt");
    if (userFile.is_open()) {
        for (const User& user : users) {
            userFile << user.username << "," << user.password << "," << user.balance << ","
                     << (user.wantsToBorrow ? "1" : "0") << "," << user.wantedCar << ","
                     << (user.wantsToReturn ? "1" : "0") << "," << user.borrowedCar << endl;
        }
        userFile.close();
    }
    
    // Save cars
    ofstream carFile("carsDB.txt");
    if (carFile.is_open()) {
        for (const Car& car : cars) {
            carFile << car.name << "," << car.count << endl;
        }
        carFile.close();
    }
}

// Check if admin login is correct
bool checkAdminLogin() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";
    cin >> password;
    
    if (username == admin.username && password == admin.password) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Wrong username or password.\n";
        return false;
    }
}

// Check if user login is correct and return user index
int checkUserLogin() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    
    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == username && users[i].password == password) {
            cout << "Login successful!\n";
            return i;
        }
    }
    cout << "Wrong username or password.\n";
    return -1;
}

// Admin functions
void addNewUser() {
    User newUser;
    cout << "Enter new username: ";
    cin >> newUser.username;
    
    // Check if username already exists
    for (const User& user : users) {
        if (user.username == newUser.username) {
            cout << "Username already exists!\n";
            return;
        }
    }
    
    cout << "Enter new password: ";
    cin >> newUser.password;
    
    newUser.balance = 10500.0;
    newUser.wantsToBorrow = false;
    newUser.wantedCar = "";
    newUser.wantsToReturn = false;
    newUser.borrowedCar = "";
    
    users.push_back(newUser);
    cout << "User added with 10500 shillings!\n";
}

void approveCarRequests() {
    for (User& user : users) {
        if (user.wantsToBorrow) {
            cout << "User " << user.username << " wants to borrow " << user.wantedCar << endl;
            cout << "Approve? (y/n): ";
            string answer;
            cin >> answer;
            
            if (answer == "y") {
                // Find the car
                bool found = false;
                for (Car& car : cars) {
                    if (car.name == user.wantedCar && car.count > 0) {
                        car.count--;
                        user.borrowedCar = user.wantedCar;
                        user.wantsToBorrow = false;
                        user.wantedCar = "";
                        cout << "Request approved!\n";
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "Car not available.\n";
                }
            }
        }
    }
}

void acceptReturnedCars() {
    for (User& user : users) {
        if (user.wantsToReturn) {
            cout << "User " << user.username << " wants to return " << user.borrowedCar << endl;
            cout << "Accept? (y/n): ";
            string answer;
            cin >> answer;
            
            if (answer == "y") {
                // Find the car and add it back
                for (Car& car : cars) {
                    if (car.name == user.borrowedCar) {
                        car.count++;
                        user.borrowedCar = "";
                        user.wantsToReturn = false;
                        cout << "Return accepted!\n";
                        break;
                    }
                }
            }
        }
    }
}

void changeAdminPassword() {
    cout << "Enter new password: ";
    cin >> admin.password;
    cout << "Password changed!\n";
}

// User functions
void showUserProfile(const User& user) {
    cout << "Username: " << user.username << endl;
    cout << "Balance: " << user.balance << " shillings" << endl;
    if (!user.borrowedCar.empty()) {
        cout << "Currently borrowed: " << user.borrowedCar << endl;
    }
}

void requestCar(User& user) {
    if (user.wantsToBorrow || !user.borrowedCar.empty()) {
        cout << "You already have a request or a borrowed car!\n";
        return;
    }
    
    if (user.balance < 5000) {
        cout << "You need at least 5000 shillings to rent a car!\n";
        return;
    }
    
    cout << "Available cars:\n";
    for (const Car& car : cars) {
        if (car.count > 0) {
            cout << car.name << ": " << car.count << " available" << endl;
        }
    }
    
    cout << "Which car do you want? ";
    string carName;
    cin >> carName;
    
    // Check if car is available
    for (const Car& car : cars) {
        if (car.name == carName && car.count > 0) {
            user.wantsToBorrow = true;
            user.wantedCar = carName;
            cout << "Request sent for " << carName << "!\n";
            return;
        }
    }
    cout << "Car not available!\n";
}

void requestReturn(User& user) {
    if (user.borrowedCar.empty()) {
        cout << "You don't have any car to return!\n";
        return;
    }
    
    user.wantsToReturn = true;
    cout << "Return request sent!\n";
}

void changeUserPassword(User& user) {
    cout << "Enter new password: ";
    cin >> user.password;
    cout << "Password changed!\n";
}

// Main program
int main() {
    loadData();
    
    while (true) {
        cout << "\n=== Car Rental System ===\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Choose: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            // Admin menu
            if (checkAdminLogin()) {
                while (true) {
                    cout << "\n=== Admin Menu ===\n";
                    cout << "1. Add New User\n";
                    cout << "2. Approve Car Requests\n";
                    cout << "3. Accept Returned Cars\n";
                    cout << "4. Change Password\n";
                    cout << "5. Logout\n";
                    cout << "Choose: ";
                    
                    int adminChoice;
                    cin >> adminChoice;
                    
                    if (adminChoice == 1) {
                        addNewUser();
                    } else if (adminChoice == 2) {
                        approveCarRequests();
                    } else if (adminChoice == 3) {
                        acceptReturnedCars();
                    } else if (adminChoice == 4) {
                        changeAdminPassword();
                    } else if (adminChoice == 5) {
                        cout << "Logged out!\n";
                        break;
                    } else {
                        cout << "Invalid choice!\n";
                    }
                }
            }
        } else if (choice == 2) {
            // User menu
            int userIndex = checkUserLogin();
            if (userIndex != -1) {
                while (true) {
                    cout << "\n=== User Menu ===\n";
                    cout << "1. View Profile\n";
                    cout << "2. Request to Rent Car\n";
                    cout << "3. Request to Return Car\n";
                    cout << "4. Change Password\n";
                    cout << "5. Logout\n";
                    cout << "Choose: ";
                    
                    int userChoice;
                    cin >> userChoice;
                    
                    if (userChoice == 1) {
                        showUserProfile(users[userIndex]);
                    } else if (userChoice == 2) {
                        requestCar(users[userIndex]);
                    } else if (userChoice == 3) {
                        requestReturn(users[userIndex]);
                    } else if (userChoice == 4) {
                        changeUserPassword(users[userIndex]);
                    } else if (userChoice == 5) {
                        cout << "Logged out!\n";
                        break;
                    } else {
                        cout << "Invalid choice!\n";
                    }
                }
            }
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
    
    saveData();
    return 0;
}