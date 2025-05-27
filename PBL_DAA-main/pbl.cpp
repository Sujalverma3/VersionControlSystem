#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
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
#include <functional>

using namespace std;

struct Commit {
    string id, branch, message, timestamp;
    vector<string> files;
    unordered_map<string, string> fileChanges; // filename -> file hash
};

vector<Commit> history;
unordered_map<string, string> fileHashes; // filename -> hash
map<string, vector<string>> commitGraph; // parent -> children
set<string> branches = {"main"};
unordered_map<string, string> users; // username -> password
vector<Commit> remoteRepo;

string currentBranch = "main";
string currentUser  = "";
const string vcsDir = ".vcs";

const string colorReset = "\033[0m";
const string colorGreen = "\033[32m";
const string colorYellow = "\033[33m";
const string colorRed = "\033[31m";

string hashFile(const string &path) {
    ifstream file(path);
    if (!file) return "";
    stringstream buffer;
    buffer << file.rdbuf();
    hash<string> hasher;
    return to_string(hasher(buffer.str()));
}

string currentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    string ts = string(ctime(&now_c));
    if (!ts.empty() && ts.back() == '\n') ts.pop_back();
    return ts;
}

void saveHashes() {
    filesystem::create_directory(vcsDir);
    ofstream out(vcsDir + "/hashes.txt");
    for (const auto &pair : fileHashes) {
        out << pair.first << " " << pair.second << "\n";
    }
}

void loadHashes() {
    ifstream in(vcsDir + "/hashes.txt");
    if (!in) return;
    string file, hash;
    while (in >> file >> hash) {
        fileHashes[file] = hash;
    }
}

void logEvent(const string &event) {
    filesystem::create_directory(vcsDir);
    ofstream log(vcsDir + "/events.log", ios::app);
    log << currentTimestamp() << ": " << event << "\n";
}

bool detectCycleUtil(const string &node, unordered_set<string> &visited, unordered_set<string> &recStack) {
    visited.insert(node);
    recStack.insert(node);

    auto it = commitGraph.find(node);
    if (it != commitGraph.end()) {
        for (const auto &neighbor : it->second) {
            if (recStack.count(neighbor)) return true;
            if (!visited.count(neighbor)) {
                if (detectCycleUtil(neighbor, visited, recStack)) return true;
            }
        }
    }
    recStack.erase(node);
    return false;
}

bool detectCycle() {
    unordered_set<string> visited;
    unordered_set<string> recStack;

    for (const auto &pair : commitGraph) {
        if (!visited.count(pair.first)) {
            if (detectCycleUtil(pair.first, visited, recStack)) {
                return true;
            }
        }
    }
    return false;
}

unordered_map<string, vector<string>> reverseGraph() {
    unordered_map<string, vector<string>> reversed;
    for (const auto &pair : commitGraph) {
        for (const auto &child : pair.second) {
            reversed[child].push_back(pair.first);
        }
    }
    return reversed;
}

vector<string> topologicalSort() {
    unordered_set<string> visited;
    stack<string> Stack;
    vector<string> sortedCommits;

    unordered_set<string> allCommits;
    for (const auto &commit : history) allCommits.insert(commit.id);
    for (const auto &pair : commitGraph) {
        allCommits.insert(pair.first);
        for (const auto &child : pair.second) allCommits.insert(child);
    }

    auto reversed = reverseGraph();

    function<void(const string &)> dfs = [&](const string &node) {
        visited.insert(node);
        for (const auto &neighbor : reversed[node]) {
            if (!visited.count(neighbor)) dfs(neighbor);
        }
        Stack.push(node);
    };

    for (const auto &commitId : allCommits) {
        if (!visited.count(commitId)) dfs(commitId);
    }

    while (!Stack.empty()) {
        sortedCommits.push_back(Stack.top());
        Stack.pop();
    }
    return sortedCommits;
}

void initRepo() {
    if (!filesystem::exists(vcsDir)) {
        filesystem::create_directory(vcsDir);
        cout << colorGreen << "Repository initialized.\n" << colorReset;
        logEvent("Repository initialized");
    } else {
        cout << colorYellow << "Repository already exists.\n" << colorReset;
    }
}

void createBranch(const string &branchName) {
    if (branches.count(branchName)) {
        cout << colorYellow << "Branch '" << branchName << "' already exists.\n" << colorReset;
    } else {
        branches.insert(branchName);
        cout << colorGreen << "Branch '" << branchName << "' created.\n" << colorReset;
        logEvent("Branch created: " + branchName);
    }
}

void switchBranch(const string &branchName) {
    if (branches.count(branchName)) {
        currentBranch = branchName;
        cout << colorGreen << "Switched to branch '" << branchName << "'.\n" << colorReset;
        logEvent("Switched to branch: " + branchName);
    } else {
        cout << colorRed << "Branch '" << branchName << "' does not exist.\n" << colorReset;
    }
}

void createCommit(const string &message) {
    if (currentUser .empty()) {
        cout << colorRed << "You must be logged in to commit.\n" << colorReset;
        return;
    }

    vector<string> files;
    unordered_map<string, string> changes;

    for (const auto &entry : filesystem::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (filename == vcsDir) continue;
            string h = hashFile(filename);
            files.push_back(filename);
            changes[filename] = h;
            fileHashes[filename] = h;
        }
    }
    saveHashes();

    Commit c;
    c.id = "c" + to_string(history.size() + 1);
    c.branch = currentBranch;
    c.message = message;
    c.timestamp = currentTimestamp();
    c.files = files;
    c.fileChanges = changes;

    history.push_back(c);

    string prevCommitId;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        if (it->branch == currentBranch && it->id != c.id) {
            prevCommitId = it->id;
            break;
        }
    }
    if (!prevCommitId.empty()) {
        commitGraph[prevCommitId].push_back(c.id);
    }

    cout << colorGreen << "Commit '" << c.id << "' created on branch '" << currentBranch << "'.\n" << colorReset;
    logEvent("Commit created: " + c.id + " on branch " + currentBranch);
}

void viewLog() {
    cout << colorYellow << "Commit History for branch '" << currentBranch << "':\n" << colorReset;
    vector<string> sortedCommits = topologicalSort();

    unordered_map<string, Commit> commitMap;
    for (const auto &c : history) {
        commitMap[c.id] = c;
    }

    for (const auto &commitId : sortedCommits) {
        if (commitMap.count(commitId) && commitMap[commitId].branch == currentBranch) {
            const auto &commit = commitMap[commitId];
            cout << colorGreen << "Commit ID: " << commit.id << "\n" << colorReset;
            cout << "Message   : " << commit.message << "\n";
            cout << "Timestamp : " << commit.timestamp << "\n";
            cout << "Files     : ";
            for (const auto &file : commit.files) cout << file << " ";
            cout << "\n\n";
        }
    }
}

void mergeBranches(const string &sourceBranch) {
    cout << "Starting merge from '" << sourceBranch << "' into '" << currentBranch << "'...\n";

    if (!branches.count(sourceBranch)) {
        cout << colorRed << "Source branch does not exist.\n" << colorReset;
        return;
    }

    string sourceLastCommitId, currentLastCommitId;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        if (it->branch == sourceBranch && sourceLastCommitId.empty()) sourceLastCommitId = it->id;
        if (it->branch == currentBranch && currentLastCommitId.empty()) currentLastCommitId = it->id;
        if (!sourceLastCommitId.empty() && !currentLastCommitId.empty()) break;
    }

    if (sourceLastCommitId.empty() || currentLastCommitId.empty()) {
        cout << colorYellow << "No commits found on one or both branches. Merge aborted.\n" << colorReset;
        return;
    }

    commitGraph[currentLastCommitId].push_back(sourceLastCommitId);

    if (detectCycle()) {
        cout << colorRed << "Merge aborted due to cycle (conflict).\n" << colorReset;
        auto &children = commitGraph[currentLastCommitId];
        children.erase(remove(children.begin(), children.end(), sourceLastCommitId), children.end());
        return;
    }

    cout << colorGreen << "Merge successful.\n" << colorReset;
    logEvent("Merged branch '" + sourceBranch + "' into '" + currentBranch + "'");
}

void registerUser (const string &username, const string &password) {
    if (users.count(username)) {
        cout << colorYellow << "User  already exists.\n" << colorReset;
        return;
    }
    users[username] = password;
    cout << colorGreen << "User  registered successfully.\n" << colorReset;
    logEvent("User  registered: " + username);
}

void loginUser (const string &username, const string &password) {
    if (!users.count(username)) {
        cout << colorRed << "User  does not exist.\n" << colorReset;
        return;
    }
    if (users[username] != password) {
        cout << colorRed << "Incorrect password.\n" << colorReset;
        return;
    }
    currentUser  = username;
    cout << colorGreen << "Logged in as " << username << ".\n" << colorReset;
    logEvent("User  logged in: " + username);
}

void logoutUser () {
    if (currentUser .empty()) {
        cout << colorYellow << "No user is currently logged in.\n" << colorReset;
        return;
    }
    cout << colorGreen << "User  " << currentUser  << " logged out.\n" << colorReset;
    logEvent("User  logged out: " + currentUser );
    currentUser .clear();
}

void createFile() {
    string filename;
    cout << "Enter filename to create: ";
    cin >> filename;
    cin.ignore();

    if (filesystem::exists(filename)) {
        cout << colorYellow << "File already exists.\n" << colorReset;
        return;
    }
    ofstream file(filename);
    if (!file) {
        cout << colorRed << "Failed to create file.\n" << colorReset;
        return;
    }
    cout << "Enter file content (end input with a single '.' on a line):\n";
    string line;
    while (true) {
        getline(cin, line);
        if (line == ".") break;
        file << line << "\n";
    }
    file.close();
    cout << colorGreen << "File '" << filename << "' created.\n" << colorReset;
}

void showHelp() {
    cout << colorYellow
         << "Available commands:\n"
         << "  init                 - Initialize repository\n"
         << "  create-file          - Create a new file\n"
         << "  commit               - Commit changes\n"
         << "  branch <name>        - Create a branch\n"
         << "  switch <name>        - Switch to a branch\n"
         << "  log                  - Show commit history\n"
         << "  merge <branch>       - Merge a branch into current\n"
         << "  register             - Register new user\n"
         << "  login                - Login user\n"
         << "  logout               - Logout user\n"
         << "  help                 - Show this help\n"
         << "  exit                 - Exit program\n"
         << colorReset;
}

int main() {
    cout << "                                       BIT BY GIT Version Control System\n";


    loadHashes();

    string command;
    while (true) {
        cout << colorGreen << currentUser  << "@" << currentBranch << "> " << colorReset;
        getline(cin, command);
        if (command.empty()) continue;

        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "exit") break;

        if (cmd == "init") {
            initRepo();
        } else if (cmd == "create-file") {
            createFile();
        } else if (cmd == "commit") {
            cout << "Enter commit message: ";
            string msg;
            getline(cin, msg);
            if (msg.empty()) {
                cout << colorYellow << "Commit message cannot be empty.\n" << colorReset;
                continue;
            }
            createCommit(msg);
        } else if (cmd == "branch") {
            string bname;
            iss >> bname;
            if (bname.empty()) {
                cout << colorYellow << "Specify branch name.\n" << colorReset;
                continue;
            }
            createBranch(bname);
        } else if (cmd == "switch") {
            string bname;
            iss >> bname;
            if (bname.empty()) {
                cout << colorYellow << "Specify branch name.\n" << colorReset;
                continue;
            }
            switchBranch(bname);
        } else if (cmd == "log") {
            viewLog();
        } else if (cmd == "merge") {
            string src;
            iss >> src;
            if (src.empty()) {
                cout << colorYellow << "Specify source branch.\n" << colorReset;
                continue;
            }
            mergeBranches(src);
        } else if (cmd == "register") {
            string user, pass;
            cout << "Username: ";
            getline(cin, user);
            cout << "Password: ";
            getline(cin, pass);
            registerUser (user, pass);
        } else if (cmd == "login") {
            string user, pass;
            cout << "Username: ";
            getline(cin, user);
            cout << "Password: ";
            getline(cin, pass);
            loginUser (user, pass);
        } else if (cmd == "help") {
            showHelp();
        } else {
            cout << colorRed << "Unknown command: " << cmd << "\n" << colorReset;
        }
    }

    cout << "Goodbye!\n";
    return 0;
}
