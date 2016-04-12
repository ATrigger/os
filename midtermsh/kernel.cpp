#include <iostream>
#include <queue>
#include <functional>
#include <vector>
using namespace std;
struct return_t;
typedef function<return_t(vector<string>)> cont_t;
struct return_t{
public:
    string syscall;
    vector<string> args;
    cont_t cont;
    return_t(string syscall,vector<string> args,cont_t cont)
        :syscall(syscall),args(args),cont(cont)
    {}
};
struct proc{
    int pid;
    vector<int> children;
    cont_t entry;
    proc(int pid, vector<int> children, function<return_t(vector<string>)> entry)
        :pid(pid),children(children),entry(entry){

    }
};


void kernel(cont_t entry, vector<string> args){
    queue<pair<proc,vector<string>>> processes;
    vector<pair<proc,int>> dead;
    vector<pair<proc,cont_t>> sleeping;
    processes.push({{0,{},entry},{}});
    while(!processes.empty() || !sleeping.empty())
    {
        for(int i=0;i<sleeping.size();i++){
            pair<proc,cont_t> prog = sleeping[i];
            for(int i=0;i<dead.size();i++){
                for(int ii=0;ii<prog.first.children.size();ii++){
                    if(dead[i].first.pid == prog.first.children[ii]){
                        processes.push({{prog.first.pid,prog.first.children,prog.second},vector<string>{to_string(dead[i].second)}});
                        //удаление ребенка
                        prog.first.children.erase(prog.first.children.begin()+ii);
                        // удаление мертвого процесса
                        dead.erase(dead.begin()+i);

                        goto exit_success;
                    }
                }
            }
            continue;
            exit_success:
            sleeping.erase(sleeping.begin()+i);
        }
        pair<proc,vector<string>> prog = processes.front();
        processes.pop();
        return_t res = prog.first.entry(prog.second);
        if(res.syscall == "exit"){
            cout << "Return code: " << args[0] << endl;
            dead.push_back({{prog.first.pid,{},{}},std::stoi(args[0])});
        }
        else if(res.syscall == "fork"){
            prog.first.children.push_back(prog.first.pid+1);
            processes.push({{prog.first.pid,prog.first.children,res.cont},{}});
            processes.push({{prog.first.pid+1,{},res.cont},{}});
        }
        else if(res.syscall == "write"){
            cout << args[0] << endl;
            processes.push({{prog.first.pid,prog.first.children,res.cont},{}});
        }
        else if(res.syscall == "wait"){
            sleeping.push_back({prog.first,res.cont});
        }
        else if(res.syscall == "chmod")
        {
            auto inode = inode("/").resolvePath(res.args[0]);
            int result = 0;
            if(inode.getOwner() == geteuid() || geteuid() == 0){
                inode.flags = std::stoi(res.args[1]);
            }
            else result = -1;
            processes.push({{prog.first.pid,prog.first.children,res.cont},{result}});
        }
    }
}

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    return 0;
}

