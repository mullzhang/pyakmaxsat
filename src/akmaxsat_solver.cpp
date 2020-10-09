#include "akmaxsat_solver.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "akmaxsat.hpp"

using namespace std;

vector<int> solve_qubo(string filename) {
    srand(time(0));

    ifstream istr(filename);
    CNF_Formula<long long> cf(istr);

#ifdef RBFS
    rbfs(cf);
#else
    fast_backtrack(cf);
#endif

    cf.printSolution();

    vector<int> solution = cf.getSolution();
    return solution;
}

/*
int main(int argc, char **argv) {
    srand(time(0));
    struct rusage starttime, endtime;    // Leo: declare rusage variables
    getrusage(RUSAGE_SELF, &starttime);  // Leo: Get start time
    if (argc < 2) {
        printf("usage: %s <cnf-file> [upper bound file]\n", argv[0]);
        return 1;
    }
    ifstream istr(argv[1]);
    CNF_Formula<long long> cf(istr);

#ifdef FORCE_LS
    char command[1000];
    char folder[1000];
    char file[1000];
    char *p = argv[1];
    while (strchr(p, '/') != NULL) p = strchr(p, '/') + 1;
    sprintf(folder, "/tmp/akmaxsat %d %s/", getpid(), p);
    sprintf(command, "mkdir \"%s\"", folder);
    system(command);
    sprintf(file, "%s%s", folder, p);
    if (cf.isWeighted())
        sprintf(command,
                "./ubcsat -alg irots -w -seed 0 -runs 10 -cutoff %d -r
bestsol "
                "-r out /dev/null -inst %s > \"%s\"",
                min(100 * cf.getNVars(), 100000), argv[1], file);
    else
        sprintf(command,
                "./ubcsat -alg irots -seed 0 -runs 10 -cutoff %d -r bestsol
-r " "out /dev/null -inst %s > \"%s\"", min(100 * cf.getNVars(), 100000),
argv[1], file); system(command); ifstream istr2(file); #else if (argc > 2) {
        ifstream istr2(argv[2]);
#endif
    ULL bestCost = cf.getHardWeight();
    string s;
    istr2 >> s;
#ifndef FORCE_LS
    if (s == "#") {
#endif
        getline(istr2, s);
        ULL t;
        string best;
        for (int i = 0; i < 10; ++i) {
            istr2 >> s >> s >> t;
            if (t < bestCost) {
                istr2 >> best;
                bestCost = (ULL)t;
            } else
                istr2 >> s;
        }
        if (best.size() > 0) {
            char *bestA = new char[best.size() + 1];
            for (int i = 0; i < (int)best.size(); ++i)
                if (best[i] == '0')
                    bestA[i + 1] = -1;
                else
                    bestA[i + 1] = 1;
            cf.saveBest(bestCost, bestA);
            delete[] bestA;
        }
#ifndef FORCE_LS
    } else {
        while (s != "Solution") istr2 >> s;
        ULL t;
        istr2 >> s >> t;
        while (istr2 >> s && s != "v")
            ;
        if (t < bestCost) {
            bestCost = (ULL)t;
            char *bestA = new char[cf.getNVars() + 1];
            for (int i = 1; i <= cf.getNVars(); ++i) {
                int t;
                istr2 >> t;
                assert(t == i || t == -i);
                bestA[i] = (t > 0 ? 1 : -1);
            }
            cf.saveBest(bestCost, bestA);
            delete[] bestA;
        }
    }
#endif
    cout << "c initialized bestCost to " << bestCost << endl;
    cout << "o " << bestCost << endl;
#ifdef FORCE_LS
    remove(file);
    remove(folder);
#else
    }
#endif
#ifdef RBFS
    rbfs(cf);
#else
    fast_backtrack(cf);
#endif
    // Leo's addition starts
    getrusage(RUSAGE_SELF, &endtime);  // Leo: Get end time
    int sec_0 = (int)starttime.ru_utime.tv_sec;
    int usec_0 = (int)starttime.ru_utime.tv_usec;  // microsecs
    int sec = (int)endtime.ru_utime.tv_sec;
    int usec = (int)endtime.ru_utime.tv_usec;  // microsecs
    double runTime =
        (double)(sec - sec_0) + (double)(usec - usec_0) / 1000000.0;
    cout.precision(12);
    cout << "c ** akmaxsat time = " << fixed << runTime << " seconds" <<
endl;
    // Leo's addition ends
    cf.printSolution();
    return 0;
}
*/