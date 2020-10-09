/*
   <akmaxsat: a (partial) (weighted) MAX-SAT solver>
    Copyright (C) 2010 Adrian Kuegel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Original starts
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "cnf_formula.hpp"

using namespace std;

void fast_backtrack(CNF_Formula<long long> &cf) {
    int *variable_stack = new int[cf.getNVars()];
    int *todo = new int[cf.getNVars()];
    int *pos = new int[cf.getNVars() + 1];
    int *variables = new int[cf.getNVars()];
    int variable_stack_len = 0;
    int p;
    int branch_cnt = 0, propagate_cnt = 0;
    long double besthvalue;
    bool found;
    bool do_lb_calc = false;
    bool firstlb = true;

    vector<pair<long long, int_c> > tv;
    for (int i = 1; i <= cf.getNVars(); ++i) {
        double hv1 =
            cf.getBinaryLength(i) * 2 + cf.getUnitLength(i) + cf.getLength(i);
        double hv2 = cf.getBinaryLength(-i) * 2 + cf.getUnitLength(-i) +
                     cf.getLength(-i);
        tv.push_back(make_pair(hv1 * hv1 + min(hv1, hv2), i));
    }
    sort(tv.begin(), tv.end());
    int sign = 0;
    int ind;
    int nvariables = cf.getNVars();
    for (int i = 0; i < nvariables; ++i) {
        variables[i] = tv[i].second;
        pos[tv[i].second] = i;
    }
    int *pit = variables + nvariables - 1;
    do {
        if (variable_stack_len == cf.getNVars()) {
            do_lb_calc = true;
            goto goback;
        }

        firstlb = false;
        if (!cf.bestMinusLowerBound()) goto goback;

        found = false;
        if (nvariables < 5000) pit = variables + nvariables - 1;
        if (pit >= variables + nvariables) pit = variables + nvariables - 1;
        for (; pit >= variables; --pit) {
            long long lneg = cf.getLength(-*pit);
            long long lpos = cf.getLength(*pit);
            // check if -i can be discarded
            if (cf.getUnitLength(*pit) >= lneg) {
                if (!cf.assignLiteral(*pit)) goto goback;
                assert(pos[*pit] == pit - variables);
                ++propagate_cnt;
                variable_stack[variable_stack_len] = *pit;
                *pit = variables[nvariables - 1];
                pos[*pit] = pit - variables;
                --nvariables;
                todo[variable_stack_len++] = 0;
                found = true;
                break;
            }
            // check if +i can be discarded
            else if (cf.getUnitLength(-*pit) >= lpos) {
                if (!cf.assignLiteral(-*pit)) goto goback;
                assert(pos[*pit] == pit - variables);
                ++propagate_cnt;
                variable_stack[variable_stack_len] = *pit;
                *pit = variables[nvariables - 1];
                pos[*pit] = pit - variables;
                --nvariables;
                todo[variable_stack_len++] = 0;
                found = true;
                break;
            }
        }
        if (found) continue;
        ind = 0;
        if (nvariables >= 3000) {
            ind = variables[nvariables - 1];
            if (cf.getW_lb(ind) + cf.getUnitLength(ind) +
                    cf.getBinaryLength(ind) >
                cf.getW_lb(-ind) + cf.getUnitLength(-ind) +
                    cf.getBinaryLength(-ind))
                sign = 1;
            else
                sign = -1;
        } else {
            besthvalue = -1;
            assert(nvariables > 0);
            for (int *it = variables + nvariables - 1; it >= variables; --it) {
                assert(pos[*it] == it - variables);
                long long lneg = cf.getLength(-*it);
                long long lpos = cf.getLength(*it);
                long double hv1 =
                    cf.getW_lb(*it) + cf.getBinaryLength(*it) + lpos;
                assert(hv1 >= 0);
                long double hv2 =
                    cf.getW_lb(-*it) + cf.getBinaryLength(-*it) + lneg;
                assert(hv2 >= 0);
                if (hv1 * hv2 + min(lpos, lneg) >= besthvalue) {
                    besthvalue = hv1 * hv2 + min(lpos, lneg);
                    ind = *it;

                    if (hv2 > hv1)
                        sign = -1;
                    else
                        sign = 1;
                }
            }
        }
        assert(ind != 0);
        todo[variable_stack_len] = -sign * ind;
        if (!cf.assignLiteral(ind * sign)) {
            if (!cf.assignLiteral(ind * -sign)) goto goback;
            todo[variable_stack_len] = 0;
        }
        ++branch_cnt;

        variable_stack[variable_stack_len++] = ind;
        p = pos[ind];
        assert(p >= 0);
        variables[p] = variables[nvariables - 1];
        pos[variables[p]] = p;
        --nvariables;
        continue;
    goback:
        while (variable_stack_len) {
            --variable_stack_len;
            cf.unassignLiteral();
            if (todo[variable_stack_len])
                if (cf.assignLiteral(todo[variable_stack_len])) {
                    todo[variable_stack_len++] = 0;
                    break;
                }
            pos[variable_stack[variable_stack_len]] = nvariables;
            variables[nvariables++] = variable_stack[variable_stack_len];
        }
    } while (variable_stack_len);
    printf("c %d branches %d propagates\n", branch_cnt, propagate_cnt);
    delete[] variable_stack;
    delete[] todo;
    delete[] variables;
    delete[] pos;
}
