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

#ifndef CLAUSES_HPP_INCLUDE
#define CLAUSES_HPP_INCLUDE

#include <assert.h>
#include <string.h>

#include <algorithm>
#include <iostream>

using namespace std;

typedef int int_c;
typedef unsigned long long ULL;

ULL MAXWEIGHT = (1ULL << 63) - 1;

/*! \file clauses.hpp Documentation of class Clauses
 */
//! The class Clauses is a data structure which stores and maintains a list of
//! clauses efficiently
class Clauses {
    // private member variable
    //! clauses contains clauses in the format \<length\> <traversal id>
    //! \<literal 1\> ... \<literal length\>
    int_c *clauses;
    //! capacity of the array clauses - a power of 2
    int capacity;
    //! capacity of the array headsFreeList - log2(capacity)
    int logcapacity;
    //! pointers to the first free block of size 2^i
    int *headsFreeList;
    //! stores which literals are currently assigned to false
    bool *assigned;
    //! number of variables
    int nVars;
    //! bit which indicates deletion of the clause
    const static int DELETED = 1 << 30;
    //! bit which indicates a marker
    const static int MARKED = 1 << 29;
    //! bit which indicates that the clause is special
    const static int SPECIAL = 1 << 28;
    //! bit which indicates that a clause was changed since it was last
    //! traversed
    const static int CHANGED = 1 << 27;
    //! maximum length of a clause
    const static int MAXLEN = (1 << 27) - 1;

    // private functions
    //! doubles the amount of memory available to store clauses
    void doubleStorage() {
        capacity <<= 1;
        ++logcapacity;
        int_c *clauses2 = new int_c[capacity];
        assert(clauses2 != NULL);
        memcpy(clauses2, clauses, sizeof(int_c) * capacity / 2);
        delete[] clauses;
        clauses = clauses2;
        int *headsFreeList2 = new int[logcapacity];
        assert(headsFreeList2 != NULL);
        memcpy(headsFreeList2, headsFreeList, sizeof(int) * (logcapacity - 1));
        delete[] headsFreeList;
        headsFreeList = headsFreeList2;
        assert(headsFreeList[logcapacity - 2] == 0);
        // add the new storage block to the buddy memory management
        headsFreeList[logcapacity - 2] = capacity / 2;
        headsFreeList[logcapacity - 1] = 0;
        clauses[capacity / 2] = 0;
    }

    //! add a free memory block of size 2^list_id to the list of empty blocks
    void addFreeBlock(int pos, int list_id) {
        clauses[pos] = headsFreeList[list_id];
        headsFreeList[list_id] = pos;
    }
    //! remove the first block of size 2^list_id from the list of empty blocks
    void popFreeBlock(int list_id) {
        assert(headsFreeList[list_id] > 0);
        headsFreeList[list_id] = clauses[headsFreeList[list_id]];
    }
    //! add special flag to clause
    /*! \param clause_id The id of the clause to which a special flag should be
     * added
     */
    void addSpecialFlag2Clause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] > 0 && (clauses[clause_id] & SPECIAL) == 0);
        clauses[clause_id] |= SPECIAL;
    }

    // public functions
   public:
    //! Clauses constructor
    Clauses() {
        assigned = NULL;
        logcapacity = 16;
        capacity = 1 << logcapacity;
        clauses = new int_c[capacity];
        headsFreeList = new int[logcapacity];
        for (int i = 0; i + 1 < logcapacity; ++i) {
            headsFreeList[i] = 1 << i;
            clauses[1 << i] = 0;
        }
        headsFreeList[logcapacity - 1] = 0;
    }

    ~Clauses() {
        delete[] clauses;
        assert(assigned != NULL);
        assigned -= nVars;
        delete[] assigned;
        delete[] headsFreeList;
    }

    //! initialize the number of variables and the assigned array
    void init(int n) {
        assert(assigned == NULL);
        nVars = n;
        assigned = new bool[2 * nVars + 1];
        memset(assigned, false, (2 * nVars + 1) * sizeof(bool));
        assigned += nVars;
    }
    //! Add a clause to the list of clauses.
    /*! \param literals specifies the literals of the clause
     *  \param length gives the length of the clause
     *  \param weight gives the weight of the clause
     *  \returns returns the clause id
     */
    int addClause(int_c *literals, int length, ULL weight) {
        assert(literals != NULL && length >= 0 && length <= MAXLEN);
        // find a free block for storing the new clause
        int pos = 0, k;
        length += 5;
        for (k = 2; k < logcapacity; ++k)
            if ((1 << k) >= length) {
                pos = headsFreeList[k];
                if (pos) break;
            }
        // increase storage as long as no large enough block was found
        while (!pos) {
            doubleStorage();
            // after doubling storage, the biggest available block has size
            // 2^(logcapacity-2)
            if ((1 << (logcapacity - 2)) >= length) {
                pos = headsFreeList[logcapacity - 2];
                assert(pos);
                k = logcapacity - 2;
                break;
            }
        }
        popFreeBlock(k);
        // now split the block until its size is the smallest power of 2 >=
        // length
        while (k > 0 && (1 << (k - 1)) >= length) {
            --k;
            addFreeBlock(pos + (1 << k), k);
        }
        length -= 5;
        clauses[pos] = length;
        *((ULL *)&clauses[pos + 1]) = *((ULL *)&clauses[pos + 3]) = weight;
        memcpy(clauses + pos + 5, literals, sizeof(int_c) * length);
        return pos;
    }
    //! lit is assigned to false
    /*! \param lit is the literal which is assigned false
     */
    void assignVariable(int lit) { assigned[lit] = true; }
    //! assignment of lit is taken back
    /*! \param lit is the literal which was assigned false and becomes
     * unassigned
     */
    void unassignVariable(int lit) { assigned[lit] = false; }
    //! decrease the length of a clause because of an assignment
    /*! \param clause_id is the id of the clause whose length is decreased
     */
    void decreaseLength(int clause_id) {
        clauses[clause_id] = (clauses[clause_id] | CHANGED) - 1;
    }
    //! increase the length of a clause because of an assignment
    /*! \param clause_id is the id of the clause whose length is increased
     */
    void increaseLength(int clause_id) {
        clauses[clause_id] = (clauses[clause_id] | CHANGED) + 1;
    }
    //! Delete clause clause_id.
    /*! \param clause_id The id of the clause to be deleted.
     */
    void deleteClause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               (clauses[clause_id] & DELETED) == DELETED &&
               (clauses[clause_id] & SPECIAL) == SPECIAL);
        int k = 2, w = getLength(clause_id) + 5;
        while ((1 << k) < w) ++k;
        addFreeBlock(clause_id, k);
    }
    //! prepare a clause for deletion
    /*! \param clause_id The id of the clause.
     *  \remark add a reference counter
     */
    void prepareDelete(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity);
        assert(clauses[clause_id + 1] == 0);
        addSpecialFlag2Clause(clause_id);
        clauses[clause_id + 5] = getLength(clause_id);
    }
    //! decrease reference counter
    /*! \param clause_id The id of the clause.
     */
    void decreaseCounter(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               (clauses[clause_id] & DELETED) == DELETED &&
               (clauses[clause_id] & SPECIAL) == SPECIAL);
        if (--clauses[clause_id + 5] == 0) deleteClause(clause_id);
    }
    //! Add a delete flag to a clause.
    /*! \param clause_id The id of the clause to be flagged
     */
    void addDeleteFlag2Clause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               (clauses[clause_id] & DELETED) == 0);
        clauses[clause_id] |= DELETED;
    }
    //! mark the clause.
    /*! \param clause_id The id of the clause to be marked
     */
    void addMarker2Clause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] > 0 && (clauses[clause_id] & MARKED) == 0);
        clauses[clause_id] |= MARKED;
    }
    //! remove the special flag from a clause
    /*! \param clause_id The id of the clause
     */
    void removeSpecialFlagFromClause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] > 0 && (clauses[clause_id] & SPECIAL));
        clauses[clause_id] ^= SPECIAL;
    }
    //! Remove marker from a clause.
    /*! \param clause_id The id of the clause.
     */
    void removeMarkerFromClause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] > 0 && (clauses[clause_id] & MARKED));
        clauses[clause_id] ^= MARKED;
    }
    //! Remove a delete flag from a clause.
    /*! \param clause_id The id of the clause.
     */
    void removeDeleteFlagFromClause(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               (clauses[clause_id] & DELETED));
        clauses[clause_id] ^= DELETED;
    }
    //! Get the delete flag of a clause.
    /*! \param clause_id The id of the clause
     *  \returns true if clause is marked as deleted, false otherwise
     */
    bool getDeleteFlag(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] != 0);
        return clauses[clause_id] & DELETED;
    }
    //! Get the marker of a clause.
    /*! \param clause_id The id of the clause
     *  \returns true if clause is marked, false otherwise
     */
    bool getMarker(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] != 0);
        return clauses[clause_id] & MARKED;
    }
    //! Get the special flag of a clause.
    /*! \param clause_id The id of the clause
     *  \returns true if clause is flagged as special, false otherwise
     */
    bool getSpecialFlag(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] != 0);
        return clauses[clause_id] & SPECIAL;
    }
    //! Get a list of literals of a clause.
    /*! \param clause_id The id of the clause.
     */
    const int_c *getLiterals(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity);
        // check if clause length has changed
        if (clauses[clause_id] & CHANGED) {
            // move the unassigned literals to the front
            clauses[clause_id] ^= CHANGED;
            int needed = getLength(clause_id);
            int *sptr = clauses + clause_id + 5;
            while (needed && !assigned[*sptr]) {
                ++sptr;
                --needed;
            }
            if (!needed) return clauses + clause_id + 5;
            int *ptr = sptr + 1;
            int c = needed;
            while (needed) {
                if (!assigned[*ptr++]) --needed;
            }
            --ptr;
            while (c--) {
                swap(*sptr, *ptr);
                if (!c) break;
                --ptr;
                while (assigned[*ptr]) --ptr;
                ++sptr;
                while (c && !assigned[*sptr]) {
                    --c;
                    ++sptr;
                }
            }
        }
        return clauses + clause_id + 5;
    }
    //! get the length of a clause.
    /*! \param clause_id The id of the clause.
     *  \returns the length of the clause
     */
    int getLength(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity &&
               clauses[clause_id] != 0);
        return clauses[clause_id] & MAXLEN;
    }
    //! get the weight of a clause.
    /*! \param clause_id The id of the clause.
     *  \returns the weight of the clause
     */
    ULL getWeight(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity);
        return *((ULL *)&clauses[clause_id + 1]);
    }
    //! get the saved weight of a clause.
    /*! \param clause_id The id of the clause.
     *  \returns the weight of the clause
     */
    ULL getSavedWeight(int clause_id) const {
        assert(clause_id > 0 && clause_id < capacity &&
               getLength(clause_id) > 0);
        return *((ULL *)&clauses[clause_id + 3]);
    }
    //! reset the weight of a clause to the saved weight.
    /*! \param clause_id The id of the clause.
     */
    void resetWeight(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               getLength(clause_id) > 0);
        assert(!getSpecialFlag(clause_id));
        if (getWeight(clause_id) == getSavedWeight(clause_id)) return;
        assert(getWeight(clause_id) < getSavedWeight(clause_id));
        *((ULL *)&clauses[clause_id + 1]) = *((ULL *)&clauses[clause_id + 3]);
        assert(getWeight(clause_id) > 0);
        clauses[clause_id] &= ~DELETED;
    }
    //! save the current weight of a clause.
    /*! \param clause_id The id of the clause.
     *  \remark saves a copy of the current weight
     */
    void saveWeight(int clause_id) {
        assert(clause_id > 0 && clause_id < capacity &&
               getLength(clause_id) > 0);
        *((ULL *)&clauses[clause_id + 3]) = *((ULL *)&clauses[clause_id + 1]);
    }
    //! increase the weight of a clause.
    /*! \param clause_id The id of the clause.
     *  \param w The weight to be added (w != 0).
     */
    void addWeight(int clause_id, ULL w, bool change_saved = false) {
        assert(clause_id > 0 && clause_id < capacity && w != 0);
        assert(!getSpecialFlag(clause_id));
        if (getWeight(clause_id) == 0) {
            assert(getDeleteFlag(clause_id) && w > 0);
            removeDeleteFlagFromClause(clause_id);
        }
        assert(w <= MAXWEIGHT - getWeight(clause_id));
        *((ULL *)&clauses[clause_id + 1]) += w;
        if (change_saved) *((ULL *)&clauses[clause_id + 3]) += w;
    }
    //! decrease the weight of a clause.
    /*! \param clause_id The id of the clause.
     *  \param w The weight to be subtracted (w != 0).
     */
    void subtractWeight(int clause_id, ULL w, bool change_saved = false) {
        assert(clause_id > 0 && clause_id < capacity && w != 0 &&
               !getDeleteFlag(clause_id));
        assert(!getSpecialFlag(clause_id));
        *((ULL *)&clauses[clause_id + 1]) -= w;
        if (change_saved) {
            assert(getSavedWeight(clause_id) >= w);
            *((ULL *)&clauses[clause_id + 3]) -= w;
        }
        if (getWeight(clause_id) == 0) addDeleteFlag2Clause(clause_id);
    }
};

#endif