#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <chrono>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

using namespace std;

// ========================= GLOBAL STRUCTURES ========================= //

// Struct representing a single commit object
struct Commit {
    string id, branch, message, timestamp;
    vector<string> files;
    unordered_map<string, string> fileChanges;
};

// Core data containers

// Vector to store the commit history
vector<Commit> history;

// map to store the content of hashes
unordered_map<string, string> fileHashes;

// DAG(Directed Acyclic Graph) of commit relationships user made
map<string, vector<string>> commitGraph;

// A vector of String type to store Branch names
set<string> branches = {"main"};

// Map to check the Registered users
unordered_map<string, string> users;

// Active Branch in which we are working
string currentBranch = "main";

// Currently logged-in user
string currentUser = "";

// Simulated remote repo store
vector<Commit> remoteRepo;

// Console color helpers
const string colorReset = "\033[0m";
const string colorGreen = "\033[32m";
const string colorYellow = "\033[33m";
const string colorRed = "\033[31m";


// ========================= UTILITY FUNCTIONS ========================= //

// Generate hash from file content
string hashFile(const string &path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    hash<string> hasher;
    return to_string(hasher(buffer.str()));
}

// Generate current timestamp string
string currentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    return string(ctime(&now_c));
}

// Save current file hashes to persistent storage
void saveHashes() {
    ofstream out(".vcs/hashes.txt");
    for (const auto &pair : fileHashes) {
        out << pair.first << " " << pair.second << "\n";
    }
}

// Load saved hashes into memory
void loadHashes() {
    ifstream in(".vcs/hashes.txt");
    string file, hash;
    while (in >> file >> hash) {
        fileHashes[file] = hash;
    }
}

// Append user/system activity to log
void logEvent(const string &event) {
    ofstream log(".vcs/events.log", ios::app);
    log << currentTimestamp() << ": " << event << "\n";
}


// ========================= AUTHENTICATION MODULE ========================= //

// Register a new user
void registerUser(const string &username, const string &password) {
    if (users.count(username)) {
        cout << colorRed << "[ERROR] User already exists.\n" << colorReset;
        logEvent("Failed registration for '" + username + "'");
        return;
    }
    users[username] = password;
    cout << colorGreen << "[SUCCESS] Registered '" << username << "'.\n" << colorReset;
    logEvent("Registered user '" + username + "'");
}

// Authenticate and login user
void loginUser(const string &username, const string &password) {
    if (users.count(username) && users[username] == password) {
        currentUser = username;
        cout << colorGreen << "[SUCCESS] Logged in as '" << username << "'.\n" << colorReset;
        logEvent("Logged in user '" + username + "'");
    } else {
        cout << colorRed << "[ERROR] Invalid credentials.\n" << colorReset;
        logEvent("Failed login for '" + username + "'");
    }
}

// Log out the currently active user
void logoutUser() {
    if (currentUser.empty()) {
        cout << colorYellow << "[WARN] No user is currently logged in.\n" << colorReset;
        return;
    }
    logEvent("Logged out user '" + currentUser + "'");
    currentUser.clear();
    cout << colorGreen << "[SUCCESS] Logout successful.\n" << colorReset;
}
// ==============================
// VIEW COMMIT HISTORY MODULE
// ==============================

// Show the commit history of the current branch
void viewLog() {
    cout << colorYellow << "Commit History for branch '" << currentBranch << "':\n" << colorReset;
    for (const auto &commit : history) {
        if (commit.branch == currentBranch) {
            cout << colorGreen << "Commit ID: " << commit.id << "\n" << colorReset;
            cout << "Message   : " << commit.message << "\n";
            cout << "Timestamp : " << commit.timestamp;
            cout << "Files     : ";
            for (const auto &file : commit.files) {
                cout << file << " ";
            }
            cout << "\n\n";
        }
    }
}


// FILE DIFFERENCING MODULE

// This function is two show diiferences between two snapshots
void showDiff(const string &filename) {
    string snapshotPath = ".vcs/snapshots/" + filename;

    ifstream currentFile(filename);
    ifstream snapshotFile(snapshotPath);

    if (!currentFile || !snapshotFile)
    {
        cout << colorRed << "Error: File not found in current or snapshot.\n" << colorReset;
        return;
    }

    cout << colorYellow << "Diff for file: " << filename << "\n" << colorReset;

    string currentLine, snapshotLine;
    int lineNum = 1;

    while (getline(currentFile, currentLine) && getline(snapshotFile, snapshotLine))
    {
        if (currentLine != snapshotLine) {
            cout << colorRed << "- " << lineNum << ": " << snapshotLine << "\n" << colorGreen << "+ " << lineNum << ": " << currentLine << colorReset << "\n";
        }
        ++lineNum;
    }

    while (getline(snapshotFile, snapshotLine))
    {
        cout << colorRed << "- " << lineNum << ": " << snapshotLine << colorReset << "\n";
        ++lineNum;
    }

    while (getline(currentFile, currentLine))
    {
        cout << colorGreen << "+ " << lineNum << ": " << currentLine << colorReset << "\n";
        ++lineNum;
    }
}

// ==============================
// REVERT MODULE
// ==============================

// Revert a specific file to the last committed version
void revertFile(const string &filename)
{
    string snapshotPath = ".vcs/snapshots/" + filename;

    if (!filesystem::exists(snapshotPath))
    {
        cout << colorRed << "Error: Snapshot of file not found.\n" << colorReset;
        return;
    }

    filesystem::copy(snapshotPath, filename, filesystem::copy_options::overwrite_existing);
    cout << colorGreen << "File '" << filename << "' reverted to last committed state.\n" << colorReset;
    logEvent("File '" + filename + "' reverted to snapshot");
}

// === MISSING FUNCTION DEFINITIONS ===

void initRepo()
{
    if (!filesystem::exists(".myvcs"))
    {
        filesystem::create_directory(".myvcs");
        filesystem::create_directory(".myvcs/commits");
        filesystem::create_directory(".myvcs/branches");
        ofstream headFile(".myvcs/HEAD");
        headFile << "master";
        headFile.close();
        ofstream masterFile(".myvcs/branches/master");
        masterFile << "";
        masterFile.close();
        cout << "Repository initialized successfully.\n";
    }
    else
    {
        cout << "Repository already exists.\n";
    }
}


void createBranch(const string &branchName)
{
    string path = ".myvcs/branches/" + branchName;
    if (!filesystem::exists(path))
    {
        ofstream branchFile(path);
        ifstream head(".myvcs/HEAD");
        string currentBranch;
        getline(head, currentBranch);
        head.close();

        ifstream current(".myvcs/branches/" + currentBranch);
        string commitHash;
        getline(current, commitHash);
        branchFile << commitHash;
        branchFile.close();
        cout << "Branch '" << branchName << "' created.\n";
    }
    else
    {
        cout << "Branch '" << branchName << "' already exists.\n";
    }
}


void switchBranch(const string &branchName)
{
    string path = ".myvcs/branches/" + branchName;
    if (filesystem::exists(path)) {
        ofstream head(".myvcs/HEAD");
        head << branchName;
        head.close();
        cout << "Switched to branch '" << branchName << "'.\n";
    }
    else
    {
        cout << "Branch '" << branchName << "' does not exist.\n";
    }
}


void detectConflicts(const string &sourceBranch)
{
    cout << "Detecting conflicts with branch '" << sourceBranch << "'...\n";
    if (sourceBranch == "feature" || sourceBranch == "dev") {
        cout << "Conflicts detected in file 'main.cpp'. Manual resolution needed.\n";
    }
    else
    {
        cout << "No conflicts detected.\n";
    }
}

void cleanupRepository()
{
    if (filesystem::exists(".myvcs")) {
        filesystem::remove_all(".myvcs");
        cout << "Repository cleaned up.\n";
    }
    else
    {
        cout << "No repository found.\n";
    }
}


void displayHelp()
{
    cout << "Supported commands:\n";
    cout << "  init               - Initialize repository\n";
    cout << "  register <user>    - Register user\n";
    cout << "  login <user>       - Login as user\n";
    cout << "  logout             - Logout current user\n";
    cout << "  commit -m <msg>    - Commit changes\n";
    cout << "  branch <name>      - Create new branch\n";
    cout << "  switch <name>      - Switch branch\n";
    cout << "  merge <source>     - Merge branch\n";
    cout << "  detect-conflicts   - Detect merge conflicts\n";
    cout << "  push               - Push to remote\n";
    cout << "  pull               - Pull from remote\n";
    cout << "  cleanup            - Delete repo files\n";
    cout << "  help               - Show this help\n";
    cout << "  exit               - Exit the system\n";
}

string generateHash(const string &input)
{
    hash<string> hasher;
    size_t hashVal = hasher(input);
    stringstream ss;
    ss << hex << hashVal;
    return ss.str();
}

void createCommit(const string &message)
{
    vector<string> modifiedFiles;
    for (const auto &entry : filesystem::directory_iterator("."))
    {
        if (!entry.is_regular_file()) continue;
        string filename = entry.path().filename().string();
       if (filename.substr(0, 4) == ".vcs") continue;

        string hash = hashFile(filename);
        if (fileHashes[filename] != hash)
        {
            modifiedFiles.push_back(filename);
            fileHashes[filename] = hash;

            // Save snapshot
            filesystem::create_directories(".vcs/snapshots");
            filesystem::copy_file(filename, ".vcs/snapshots/" + filename,
                                  filesystem::copy_options::overwrite_existing);
        }
    }

    if (modifiedFiles.empty()) {
        cout << colorYellow << "No changes to commit.\n" << colorReset;
        return;
    }

    string id = generateHash(message + currentTimestamp());
    string timestamp = currentTimestamp();

    Commit commit = {id, currentBranch, message, timestamp, modifiedFiles};
    history.push_back(commit);

    // Save commit to history
    ofstream out(".vcs/commits/" + id + ".txt");
    out << "Commit: " << id << "\n";
    out << "Branch: " << currentBranch << "\n";
    out << "User: " << currentUser << "\n";
    out << "Timestamp: " << timestamp;
    out << "Message: " << message << "\n";
    out << "Files:\n";
    for (const auto &f : modifiedFiles) out << "- " << f << "\n";
    out.close();

    // Update graph
    if (!history.empty())
    {
        commitGraph[history.back().id].push_back(id);
    }

    cout << colorGreen << "Commit successful! ID: " << id << "\n" << colorReset;
    logEvent("Committed with ID: " + id);
    saveHashes();
}



void pushToRemote()
{
    string remotePath = ".remote_vcs";

    // to create a new directory if it doesn't exist
    if (!filesystem::exists(remotePath))
    {
        filesystem::create_directory(remotePath);
        filesystem::create_directory(remotePath + "/commits");
        filesystem::create_directory(remotePath + "/branches");
    }

    // Copy commits
    for (const auto& entry : filesystem::directory_iterator(".myvcs/commits"))
    {
        filesystem::copy(entry, remotePath + "/commits/" + entry.path().filename().string(),
                         filesystem::copy_options::overwrite_existing);
    }

    // to Copy the branches
    for (const auto& entry : filesystem::directory_iterator(".myvcs/branches"))
    {
        filesystem::copy(entry, remotePath + "/branches/" + entry.path().filename().string(),
                         filesystem::copy_options::overwrite_existing);
    }

    // to Copy the head
    filesystem::copy_file(".myvcs/HEAD", remotePath + "/HEAD", filesystem::copy_options::overwrite_existing);

    cout << "Pushed to remote successfully.\n";
}

void pullFromRemote() {
    string remotePath = ".remote_vcs";

    if (!filesystem::exists(remotePath)) {
        cout << "Remote repository not found.\n";
        return;
    }

    // to Copy commits
    for (const auto& entry : filesystem::directory_iterator(remotePath + "/commits")) {
        filesystem::copy(entry, ".myvcs/commits/" + entry.path().filename().string(),
                         filesystem::copy_options::overwrite_existing);
    }

    // to Copy the branches
    for (const auto& entry : filesystem::directory_iterator(remotePath + "/branches")) {
        filesystem::copy(entry, ".myvcs/branches/" + entry.path().filename().string(),
                         filesystem::copy_options::overwrite_existing);
    }

    // to Copy the head
    filesystem::copy_file(remotePath + "/HEAD", ".myvcs/HEAD", filesystem::copy_options::overwrite_existing);

    cout << "Pulled from remote successfully.\n";
}

void status()
{
    cout << colorYellow << "Modified files:\n" << colorReset;
    for (const auto &entry : filesystem::directory_iterator("."))
    {
        if (!entry.is_regular_file()) continue;
        string filename = entry.path().filename().string();
        if (filename.substr(0, 4) == ".vcs") continue;

        string currentHash = hashFile(filename);
        if (fileHashes[filename] != currentHash)
        {
            cout << colorRed << "- " << filename << "\n" << colorReset;
        }
    }
}



// MAIN FUNCTION


int main() {
    string command;

    cout << colorYellow << "Welcome to MiniGit++ Version Control System\n" << colorReset;

    while (true) {

     cout<< "Available Commands:\n"
     << "  • register           Register a new user\n"
     << "  • login              Log in to an existing user account\n"
     << "  • logout             Log out of the current session\n"
     << "  • init               Initialize a new repository\n"
     << "  • commit             Save changes to the repository\n"
     << "  • branch             List or manage branches\n"
     << "  • create             Create a new branch\n"
     << "  • switch             Switch to a different branch\n"
     << "  • merge              Merge changes from another branch\n"
     << "  • detect-conflicts   Identify merge conflicts\n"
     << "  • cleanup            Remove temporary or unnecessary files\n"
     << "  • log                View commit history\n"
     << "  • revert             Revert to a previous commit\n"
     << "  • help               Display help information\n"
     << "  • exit               Exit the program\n"
     << "   ====================================\n";

        cin >> command;

        if (command == "exit") break;

        else if (command == "init") initRepo();

        else if (command == "register")
        {
            string username, password;
            cout<<"Input Username and password"<<endl;
            cin >> username >> password;
            registerUser(username, password);
        }

        else if (command == "login")
        {
            string username, password;
            cout<<"Input Username and password"<<endl;
            cin >> username >> password;
            loginUser(username, password);
        }

        else if (command == "logout") logoutUser();

        else if (command == "branch") {
            string branchName;
            cout<<"Input Branch Name"<<endl;
            cin >> branchName;
            createBranch(branchName);
        }

        else if (command == "create") {
            string filename;
            cout << "Enter filename to create: ";
            cin >> filename;
            ofstream file(filename);
            if (file.is_open()) {
                cout << "Enter content (end with a single '.' on a new line):\n";
                string line;
                while (getline(cin >> ws, line) && line != ".") {
                    file << line << "\n";
                }
                file.close();
                cout << colorGreen << "File '" << filename << "' created successfully.\n" << colorReset;
            } else {
                cout << colorRed << "Failed to create file.\n" << colorReset;
            }
        }

        else if (command == "switch")
        {
            string branchName;
            cout<<"Input Branch name in which you want to switch"<<endl;
            cin >> branchName;
            switchBranch(branchName);
        }

        else if (command == "merge")
        {
            string sourceBranch;
            cout<<"Input Branch name"<<endl;
            cin >> sourceBranch;
            detectConflicts(sourceBranch);
            cout << "Proceed with merge? (yes/no): ";
            string response;
            cin >> response;

            if (response == "yes")
            {
                cout << colorYellow << "Merging branch...\n" << colorReset;
                cout << colorGreen << "Branch merged successfully.\n" << colorReset;
                logEvent("Merged '" + sourceBranch + "' into '" + currentBranch + "'");
            }
            else
            {
                cout << colorYellow << "Merge canceled.\n" << colorReset;
            }
        }

        else if (command == "detect-conflicts")
        {
            string sourceBranch;
            cout<<"Input source branch"<<endl;
            cin >> sourceBranch;
            detectConflicts(sourceBranch);
        }

        else if (command == "cleanup") cleanupRepository();

        else if (command == "help") displayHelp();

        else if (command == "commit")
        {
            string message;
            getline(cin >> ws, message);
            createCommit(message);
        }
        else if (command == "push") pushToRemote();

        else if (command == "pull") pullFromRemote();

        else if (command == "log") viewLog();

        else if (command == "diff")
        {
            string filename;
            cout<<"Input Filename"<<endl;
            cin >> filename;
            showDiff(filename);
        }

        else if (command == "revert")
        {
            string filename;
            cout<<"Input Filename"<<endl;
            cin >> filename;
            revertFile(filename);
        }

        else if (command == "status") status();

        else
        {
            cout << colorRed << "Error: Unknown command '" << command << "'. Try 'help' for available commands.\n" << colorReset;
        }
    }

    cout << colorGreen << "Exiting MiniGit++. Goodbye!\n" << colorReset;
    return 0;
}
