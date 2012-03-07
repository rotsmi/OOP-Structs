
/*
 * Copyright 2012 Kenneth Chiu <kchiu@cs.binghamton.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * Run with
 * 
 *    -i iterations
 *
 * to do a stress test for the given number of iterations.
 * Use
 *
 *    -s seed
 *
 * to give a different seed.  Use a seed of 0 to seed
 * with the current time.
 */



#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <sys/time.h>
#include <vector>

#include "Person.hpp"
#include "List.hpp"



/*
 * Person hierarchy print functions.  These are not intended to be member
 * functions, but more like non-member helper functions.
 */

void
Person_print(Person *p) {
    printf("Name: %s\n", p->name);
    printf("Gender: %c\n", p->gender);
}

void
Student_print(Student *s) {
    printf("Major: %s\n", s->major);
}

void
Under_print(Under *u) {
    printf("Year: %d\n", u->year);
}

void
Grad_print(Grad *g) {
    printf("Degree: %s\n", g->degree);
}

void
Professor_print(Professor *p) {
    printf("Office: %s\n", p->office);
}



/*
 * List test functions.
 */

typedef Person *PersonPtr;

// Comparison function, used later.
bool
PersonPtr_less(const PersonPtr &p1, const PersonPtr &p2) {
    return strcmp(p1->name, p2->name) < 0;
}
// This "instantiates" the template.  Note that there is no semicolon.
List_DEFINE(PersonPtr)

// MyClass List
struct MyClass {
    double num;
};
bool
MyClass_less(const MyClass &o1, const MyClass &o2) {
    return o1.num < o2.num;
}
List_DEFINE(MyClass)

// int List
bool
int_less(const int &i1, const int &i2) {
    return i1 < i2;
}
List_DEFINE(int)

// Stress test List
struct Stress {
    int val;
    Stress(int _v) : val(_v){}
};
bool
Stress_less(const Stress& o1, const Stress& o2) {
    return o1.val < o2.val;
}
List_DEFINE(Stress)


/*
 * Additional test functions for List.
 */

void traverse(List_PersonPtr &l, int level);
void traverse2(int level);
void check(List_Stress *list, std::list<int> &mirror);

// Returns a random number in the given range, inclusive.
inline size_t rand_int(size_t low, size_t high) {
    return (high - low + 1)*drand48() + low;
}

void
print(List_PersonPtr *list) {
    printf("---- Print begin\n");
    List_PersonPtr_Iterator it = list->begin(list);
    while (!List_PersonPtr_Iterator_equal(it, list->end(list))) {
        printf("    %s\n", it.deref(&it)->name);
        it.inc(&it);
    }
    printf("---- Print end\n");
}



/*
 * Main.
 */

int
main(int argc, char *argv[]) {

    /*
    // Code to test the rand_int() function.
    {
        size_t cnts[2] = {0, 0};
        for (int i = 0; i < 10000; i++) {
            size_t n = rand_int(0, 1);
            assert(n >= 0 && n <= 1);
            cnts[n]++;
        }
        printf("%zu 0s and %zu 1s\n", cnts[0], cnts[1]);
        exit(0);
    }
    */

    int c;
    int iterations = 0;
    long seed = 1234;
    while ((c = getopt(argc, argv, "s:i:")) != EOF) {
        switch (c) {
            case 'i':
                iterations = atoi(optarg);
                break;
            case 's':
                {
                    seed = atol(optarg);
                    if (seed == 0) {
                        timeval tv;
                        int ec = gettimeofday(&tv, 0);
                        assert(ec == 0);
                        seed = tv.tv_sec ^ tv.tv_usec;
                        // Not sure of the behavior under 64-bits, so mix things
                        // up everywhere.
                        if (sizeof(seed) > 4) {
                            seed = seed ^ (seed >> 32) ^ (seed << 32);
                        }
                    }
                    printf("---- Seeding with 0x%lx\n", seed);
                }
                break;
            case '?':
                fprintf(stderr, "Unrecog.\n");
                exit(1);
        }
    }

    srand48(seed);



    /**************************************************************************
     * Test Part 1, Person hierarchy.
     */

    Person *p1 = 0;
    {
        /*
         * Create an Under object.
         */

        printf("---- Under\n");

        char *n = strdup("Jane");
        char *major = strdup("CS");
        // This allocates and constructs an Under object.  Pass allocated
        // strings to prevent the implementation from just saving the
        // pointers, which would be possible if we passed literals.
        p1 = (Person *) Under_new(n, 'F', major, 4);
        free(n);
        free(major);
        
        // Test field access.  Prints:
        //     Name: <name>
        //     Gender: <gender>
        Person_print(p1);
        // Test method call.  Note that Grad and Under print the same thing
        // for work, and so can share the implementation.
        p1->work(p1); // Prints "<name> studies <major>."

        // Test downcasting.  Must return null if not a Student.
        Student *s = Person_downcast_Student(p1);
        Student_print(s); // Prints "Major: <major>"
        // Prints "<name> graduates in <N> years and finds a job in <major>
        // or goes to grad school."  Assume year 4 means one more year to
        // go.
        s->graduate(s);

        // Test downcast.  Must return null if not an Under.
        Under *u = Person_downcast_Under(p1);
        assert(Student_downcast_Under(s) == u);
        Under_print(u); // Prints "Year: <year>"

        // Check that upcast still works right.
        assert((Person *) u == p1);

        // Check that other downcasts fail.
        assert(Person_downcast_Grad(p1) == 0);
        assert(Person_downcast_Professor(p1) == 0);
        assert(Student_downcast_Grad(s) == 0);
    }

    Person *p2 = 0;
    {
        /*
         * Create an Grad object.
         */

        printf("---- Grad\n");

        char *n = strdup("John");
        char *major = strdup("Biology");
        char *degree = strdup("PhD");
        p2 = (Person *) Grad_new(n, 'M', major, degree);
        free(n);
        free(major);
        free(degree);
        
        Person_print(p2);
        p2->work(p2); // Same as Under.

        Student *s = Person_downcast_Student(p2);
        Student_print(s);
        // Prints "<name> graduates with a <degree> and finds a job in <major>."
        s->graduate(s);

        Grad *g = Person_downcast_Grad(p2);
        assert(Student_downcast_Grad(s) == g);
        Grad_print(g); // Prints "Degree: <degree>"

        // Check that other downcasts fail.
        assert(Person_downcast_Under(p2) == 0);
        assert(Person_downcast_Professor(p2) == 0);
        assert(Student_downcast_Under(s) == 0);
    }

    Person *p3 = 0;
    {
        /*
         * Create an Professor object.
         */

        printf("---- Professor\n");

        char *name = strdup("Mary");
        char *office = strdup("T-10");
        p3 = (Person *) Professor_new(name, 'F', office);
        free(name);
        free(office);
        
        Person_print(p3);
        p3->work(p3); // Prints "<name> teaches."

        Professor *p = Person_downcast_Professor(p3);
        Professor_print(p); // Prints "Office: <office>"
        // Must print "<name> does research in <topic>."
        p->research(p, "computers");
        p->research(p, "frogs");

        // Check that other downcasts fail.
        assert(Person_downcast_Student(p3) == 0);
        assert(Person_downcast_Under(p3) == 0);
        assert(Person_downcast_Grad(p3) == 0);
    }

    Person *p4 = (Person *) Under_new("Dave", 'M', "Chemistry", 2);

    /**************************************************************************
     * Test Part 2, List template.
     */

    /*
     * Test with Person.
     */

    {
        // This creates a List of pointers to Person objects.  It
        // corresponds to what in real C++ might be std::list<Person *>.
        // Note that it is not a List of Person objects, but rather of
        // _pointers_ to Person objects.
        List_PersonPtr *list = List_PersonPtr_new(PersonPtr_less);

        // Should print "---- List_PersonPtr, 15".  Note that 15
        // is the length of "List_PersonPtr" including the 0 byte
        // at the end..
        printf("---- %s, %d\n", list->type, (int) sizeof(list->type));

        // Insert people into the List.
        list->push_front(list, p1);
        print(list);
        list->push_front(list, p2);
        print(list);
        list->push_front(list, p3);
        print(list);
        list->push_front(list, p4);
        print(list);

        // Check iterator equality.
        {
            // Returns an iterator pointing to the first element.
            List_PersonPtr_Iterator it1 = list->begin(list);
            // Returns an iterator pointing to one PAST the last element.
            // This iterator is obviously conceptual only.  It cannot be
            // dereferenced.
            List_PersonPtr_Iterator it2 = list->end(list);

            // inc() advances the iterator forward.  dec() decrements the
            // iterator so it points to the preceding element.
            it1.inc(&it1); // Second node now.
            it1.inc(&it1); // Third node now.
            it2.dec(&it2); // Fourth node now.
            it2.dec(&it2); // Third node now.
            assert(List_PersonPtr_Iterator_equal(it1, it2));
            it2.dec(&it2); // Second node now.
            it2.dec(&it2); // First node now.
            assert(List_PersonPtr_Iterator_equal(list->begin(list), it2));
        }

        // Simple sort check.
        {
            list->sort(list);
            printf("---- Should be sorted now\n");
            print(list);
        }

        // Check push_front return value.
        {
            printf("---- Test push_front() return.\n");
            // Insert returns an interator.  The iterator points to the
            // element, and can be used to erase the element.
            Person *pp = (Person *) Professor_new("Larry", 'M', "F9");
            List_PersonPtr_Iterator it = list->push_front(list, pp);
            Person_print(it.deref(&it));
            list->erase(list, it);
            pp->delet(pp);
        }

        // Print the whole thing now, to verify ordering.
        printf("---- Before erasures.\n");

        // Iterate through the whole List, and call work() on each Person.
        // deref() returns what the iterator was pointing to.
        for (List_PersonPtr_Iterator it = list->begin(list);
         !List_PersonPtr_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            Person *p = it.deref(&it);
            p->work(p);
        }

        // Test multiple traversals of the same List.
        printf("---- Multiple traversals\n");
        traverse(*list, 3);

        // Test multiple List at the same time.
        printf("---- Multiple List objects\n");
        traverse2(4);

        /*
         * Test some erasures.
         */

        // Erase first element.
        list->erase(list, list->begin(list));
        List_PersonPtr_Iterator it = list->end(list);
        it.dec(&it); // it now points to last element.
        it.dec(&it); // it now points to penultimate element.
        list->erase(list, it);

        printf("---- After erasures.\n");

        // Iterate through the whole List, and call work() on each
        // Person.
        for (List_PersonPtr_Iterator it = list->begin(list);
         !List_PersonPtr_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            Person *p = it.deref(&it);
            p->work(p);
        }

        // Test iterator validity.
        {
            // Iterators must be valid even when other things are inserted.
            printf("---- Test iterator non-invalidation\n");

            Person *pp = (Person *) Professor_new("Zeke", 'M', "F9");
            // Will be at the end.
            List_PersonPtr_Iterator it = list->push_back(list, pp);
            // Delete first, iterator should still be valid.
            List_PersonPtr_Iterator b = list->begin(list);
            Person *tmp = b.deref(&b); // Save, so we can reinsert.
            list->erase(list, list->begin(list)); // Erase it.
            Person_print(it.deref(&it)); // This should still be valid.
            list->push_back(list, tmp); // Reinsert.
            list->erase(list, it); // Erase.
            pp->delet(pp); // Cleanup.
        }

        // Must not delete the actual Person objects.  Note that we cannot use
        // 'delete', because it is a reserved word (keyword).
        list->delet(list);

        // Note that these functions must cleanup after the derived object.  For
        // example, if p1 is a Under, delet() must cleanup after Under.  These are
        // what we call "virtual destructors", which we will talk about more later.
        p1->delet(p1);
        p2->delet(p2);
        p3->delet(p3);
        p4->delet(p4);
    }

    /*
     * Test List with MyClass.
     */

    {
        MyClass m1, m2, m3, m4, m5;
        m1.num = 0;
        m2.num = 3;
        m3.num = 1;
        m4.num = 2;
        m5.num = -999;
        List_MyClass* list = List_MyClass_new(MyClass_less);

        // Should print "---- List_MyClass, 13".  Note that 13
        // is the length of "List_MyClass" including the 0 byte
        // at the end..
        printf("---- %s, %d\n", list->type, (int) sizeof(list->type));

        List_MyClass_Iterator it;
        // Empty container, should print nothing.
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
         !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        list->push_back(list, m1);
        list->push_front(list, m2);
        list->push_back(list, m3);
        list->push_back(list, m4);
        list->push_front(list, m5);
        list->sort(list);

        // Should print: ---- List_MyClass contains: -999.0 0.0 1.0 2.0 3.0 
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
         !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        // Erase the first element.
        list->erase(list, list->begin(list));
        // Should print: "---- List_MyClass contains: 0.0 1.0 2.0 3.0"
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
         !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        // Erase the new first element.
        list->erase(list, list->begin(list));
        // Should print "1.0 2.0 3.0".
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
         !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        // Erase the element in the end.
        it = list->end(list);
        it.dec(&it); // It now points to last element.
        list->erase(list, it);
        // Should print "1.0 2.0".
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
         !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        // Erase the last element.
        list->erase(list, list->begin(list));
        // Should print: "2.0".
        printf("---- List_MyClass contains: ");
        for (it = list->begin(list);
                !List_MyClass_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            MyClass m = it.deref(&it);
            printf("%3.1f ", m.num);
        }
        printf("\n");

        list->delet(list);
    }

    /*
     * Test List with plain int.
     */

    {
        List_int* list = List_int_new(int_less);
        // Should print "---- List_int, 9".  Note that 9 is the length
        // of "List_int", including the 0 byte at the end.
        printf("---- %s, %d\n", list->type, (int) sizeof(list->type));

        List_int_Iterator it; 
        // Empty container, should print nothing.
        printf("---- List_int contains: ");
        for (it = list->begin(list);
         !List_int_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            printf("%d ", it.deref(&it));
        }
        printf("\n");

        list->push_back(list, 4);
        list->push_back(list, 3);
        list->push_back(list, 0);
        list->push_back(list, 2);
        list->push_back(list, 1);
        list->sort(list);

        // Should print "0 1 2 3 4".
        printf("---- List_int contains: ");
        for (it = list->begin(list);
         !List_int_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            printf("%d ", it.deref(&it));
        }
        printf("\n");

        // Insert dupes.
        list->push_back(list, 4);
        list->push_back(list, 3);
        list->push_back(list, 0);
        list->push_back(list, 2);
        list->push_back(list, 1);
        list->sort(list);
        // Should print "0 0 1 1 2 2 3 3 4 4".
        printf("---- List_int contains: ");
        for (it = list->begin(list);
         !List_int_Iterator_equal(it, list->end(list)); it.inc(&it)) {
            printf("%d ", it.deref(&it));
        }
        printf("\n");

        // Erase the first element.
        list->erase(list, list->begin(list));

        // Erase the new first element.
        list->erase(list, list->begin(list));

        // Erase the element in the end.
        it = list->end(list);
        it.dec(&it); // It now points to last element.
        list->erase(list, it);
        // Should print: ---- List_int contains: 1 1 2 2 3 3 4 
        printf("---- List_int contains: ");
        for (it = list->begin(list); !List_int_Iterator_equal(it, list->end(list)); it.inc(&it)) 
            printf("%d ", it.deref(&it));
        printf("\n");

        // Erase all elements.
        list->erase(list, list->begin(list));
        list->erase(list, list->begin(list));
        // Should print: ---- List_int contains: 2 2 3 3 4 
        printf("---- List_int contains: ");
        for (it = list->begin(list); !List_int_Iterator_equal(it, list->end(list)); it.inc(&it)) 
            printf("%d ", it.deref(&it));
        printf("\n");

        list->delet(list);
    }

    /*
     * Stress test List.
     */

    if (iterations > 0) {

        List_Stress* list = List_Stress_new(Stress_less);

        // Used to mirror the operations, to verify that the List
        // implementation is behaving correctly.
        typedef std::list<int> mirror_t;
        mirror_t mirror;

        typedef std::pair<List_Stress_Iterator, mirror_t::iterator> pair_t;
        typedef std::list<pair_t> iters_t;
        // Used to store a list of iterators, to test iterators.  We keep
        // one iterator per element.  This invariant is not crucial to the
        // test, but it's a simple way of figuring out how many iterators
        // to keep.  Note that it's possible that two iterators point to
        // the same element (since we also test incrementing and
        // decrementing the iterators), but we maintain the number.
        iters_t iters;

        std::cout << "---- Starting stress test with " << iterations
         << " iterations:" << std::endl;

        const int N = iterations;

        // Keep track of some statistics, to verify that the test
        // is behaving properly.
        int n_inserted = 0, n_erased = 0, n_iters_changed = 0, n_empty = 0,
         n_sorted = 0, n_erase_iterator_is_end = 0;
        double avg_size = 0;

        for (int i = 0; i < N; ++i) {

            // printf("======== Iteration %d\n", i);

            double op = drand48();

            // The probability of removal should be slightly higher than the
            // probability of insertion so that the List is often empty.
            if (op < .35) {

                // Insert an element.

                List_Stress_Iterator it;
                mirror_t::iterator mirror_it;
                int v = rand_int(0, 1000000);
                if (drand48() < .5) {
                    // Do push_front().
                    // printf("Pushing %d to the front.\n", v);
                    mirror.push_front(v);
                    mirror_it = mirror.begin();
                    it = list->push_front(list, v);
                } else {
                    // Do push_back().
                    // printf("Pushing %d to the back.\n", v);
                    mirror.push_back(v);
                    mirror_it = --mirror.end();
                    it = list->push_back(list, v);
                }

                // Store an iterator for the new element.
                iters.push_back(make_pair(it, mirror_it));

                ++n_inserted;
                 
            } else if (op < .85) {

                // Erase an element.
                if (iters.size() != 0) {

                    // Pick a random iterator to use for erasing.
                    int index = rand_int(0, iters.size() - 1);
                    iters_t::iterator iters_it = iters.begin();
                    while (index--) { ++iters_it; }

                    List_Stress_Iterator &it(iters_it->first);
                    // The iterator should not be the end iterator.  If it
                    // is, we simply skip this operation.  Note that we
                    // cannot keep picking iterators till we find one that
                    // is not the end iterator, because there might not be
                    // any such iterators.
                    if (!List_Stress_Iterator_equal(it, list->end(list))) {

                        mirror_t::iterator &mirror_it(iters_it->second);

                        // printf("Removing %d element.\n", index + 1);

                        // Find all other iterators pointing to the same
                        // element, and remove.  We need to do this since
                        // those iterators will be invalidated once this
                        // iterator is removed.  Keep track of how many
                        // were removed, though, so we can add back that
                        // many iterators, so as to keep the same number of
                        // iterators as there are elements.
                        size_t n_removed = 0;
                        iters_t::iterator next;
                        for (iters_t::iterator i = iters.begin(); i != iters.end(); i = next) {
                            iters_t::iterator tmp = i; next = ++i; i = tmp;
                            List_Stress_Iterator &it2(i->first);
                            if (i != iters_it && List_Stress_Iterator_equal(it, it2)) {
                                iters.erase(i);
                                n_removed++;
                            }
                        }

                        mirror.erase(mirror_it);
                        list->erase(list, it);
                        iters.erase(iters_it);

                        // Create new iterators to replace the deleted ones.
                        for (size_t i = 0; i < n_removed; i++) {
                            List_Stress_Iterator it = list->begin(list);
                            mirror_t::iterator mirror_it = mirror.begin();
                            // New iterator could point to end.
                            int index = rand_int(0, mirror.size());
                            while (index--) {
                                assert(!List_Stress_Iterator_equal(it, list->end(list)));
                                assert(mirror_it != mirror.end());
                                it.inc(&it); ++mirror_it;
                            }
                            iters.push_back(make_pair(it, mirror_it));
                        }

                        ++n_erased;

                    } else {

                        ++n_erase_iterator_is_end;
                    }
                }

            } else if (op < .98) {

                // Increment or decrement an iterator.

                // Size of containers should be same
                assert(iters.size() == mirror.size());

                // If the container is empty, skip
                if (iters.size() != 0) {

                    // Pick a random index
                    int index = rand_int(0, iters.size() - 1);
                    iters_t::iterator iit = iters.begin();
                    while(index--) { ++iit; }

                    List_Stress_Iterator &it(iit->first);
                    mirror_t::iterator &mirror_it(iit->second);

                    // If it is the begin(), then only increment.  If it is
                    // end(), then only decrement.  Otherwise, pick either
                    // forward or backward.
                    if (List_Stress_Iterator_equal(it, list->begin(list))) {
                        // printf("Incrementing iterator pointing to %d.\n", int(it.deref(&it).val));
                        it.inc(&it);
                        ++mirror_it;
                    } else if (List_Stress_Iterator_equal(it, list->end(list))) {
                        // printf("Decrementing end iterator.\n");
                        it.dec(&it);
                        --mirror_it;
                    } else {
                        if (rand_int(0, 1)%2 == 0) {
                            // printf("Incrementing iterator pointing to %d.\n", int(it.deref(&it).val));
                            it.inc(&it);
                            ++mirror_it;
                        } else {
                            // printf("Decrementing iterator pointing to %d.\n", int(it.deref(&it).val));
                            it.dec(&it);
                            --mirror_it;
                        }
                    }

                    ++n_iters_changed;
                }

            } else {
                
                // Sort the container.

                list->sort(list);
                mirror.sort();
                ++n_sorted;
            }

            assert(iters.size() == mirror.size());
            avg_size += double(mirror.size())/N;

            if (iters.size() == 0) {
                ++n_empty;
            }

            /*
            {
                printf("---- List contents\n");
                List_Stress_Iterator it = list->begin(list);
                while (!List_Stress_Iterator_equal(it, list->end(list))) {
                    printf("%d\n", it.deref(&it).val);
                    it.inc(&it);
                }
                printf("---- Mirror contents\n");
                for (mirror_t::iterator it = mirror.begin(); it != mirror.end(); ++it) {
                    std::cout << *it << std::endl;
                }
                printf("---- Iterators\n");
                for (iters_t::iterator it = iters.begin(); it != iters.end(); ++it) {
                    List_Stress_Iterator &lit(it->first);
                    if (!List_Stress_Iterator_equal(lit, list->end(list))) {
                        std::cout << "list it: " << it->first.deref(&it->first).val << ", ";
                        std::cout << "mirror it: " << *it->second << std::endl;
                    } else {
                        std::cout << "end it." << std::endl;
                    }
                }
            }
            */

            check(list, mirror);
        }

        std::cout << "---- Finished stress test." << std::endl;
        std::cout << "----     inserted: " << n_inserted << " times" << std::endl;
        std::cout << "----     erased: " << n_erased << " times" << std::endl;
        std::cout << "----     iterators changed: " << n_iters_changed << " times" << std::endl;
        std::cout << "----     sorted: " << n_sorted << " times" << std::endl;
        std::cout << "----     empty count: " << n_empty << std::endl;
        std::cout << "----     end iterator chosen for erase: " << n_erase_iterator_is_end << std::endl;
        std::cout << "----     avg size: " << avg_size << std::endl;

        list->delet(list);
    }
}

void
check(List_Stress *list, std::list<int> &mirror) {

    // Check if the reference container and stress container are identical.
    List_Stress_Iterator it = list->begin(list);
    std::list<int>::iterator mit = mirror.begin();

    for( ; !List_Stress_Iterator_equal(it, list->end(list)) && mit != mirror.end(); it.inc(&it), ++mit) {
        Stress e = it.deref(&it);
        if (e.val != *mit) {
            fprintf(stderr, "Reference tree and test tree differ.\n");
            abort();
        }
    }

    if(!List_Stress_Iterator_equal(it, list->end(list)) || mit!=mirror.end()) {
        fprintf(stderr, "Reference tree and test tree differ.\n");
        abort();
    }
}

// Test single list being traversed by multiple iterators simultaneously.
void
traverse(List_PersonPtr &list, int level) {
    for (List_PersonPtr_Iterator it = list.begin(&list);
     !List_PersonPtr_Iterator_equal(it, list.end(&list)); it.inc(&it)) {
        Person_print(it.deref(&it));
        if (level != 0) {
            traverse(list, level - 1);
        }
    }
}

// Test multiple lists.
void
traverse2(int level) {

    List_PersonPtr *list = List_PersonPtr_new(PersonPtr_less);

    struct Local {
        static Person *person() {
            char name[30];
            sprintf(name, "Jane%d", int(10000*drand48()));
            return (Person *) Professor_new(name, 'F', "T-10");
        }
    };

    for (int i = 0; i < 4; i++) {
        Person *p = Local::person();
        List_PersonPtr_Iterator it = list->push_back(list, p);
        // If a duplicate (unlikely but possible), free it, because it
        // didn't get inserted.
        if (List_PersonPtr_Iterator_equal(it, list->end(list))) {
            p->delet(p);
        }
    }

    for (List_PersonPtr_Iterator it = list->begin(list);
     !List_PersonPtr_Iterator_equal(it, list->end(list));
     it.inc(&it)) {
        Person_print(it.deref(&it));
        if (level != 0) {
            traverse2(level - 1);
        }
    }

    // Remove from List before deleting, to avoid having pointers to
    // deleted Person objects in the List, which can mess up
    // some algorithms for incrementing.
    {
        std::vector<Person *> del_list;
        for (List_PersonPtr_Iterator it = list->begin(list);
         !List_PersonPtr_Iterator_equal(it, list->end(list));
         it.inc(&it)) {
            del_list.push_back(it.deref(&it));
        }

        list->delet(list);

        for (size_t i = 0; i < del_list.size(); i++) {
            del_list.at(i)->delet(del_list.at(i));
        }
    }
}
