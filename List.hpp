/*
 * Copyright 2012 Alex Garcia <rotsmi@gmail.com>
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

#ifndef LISTDEFINE_HPP
#define LISTDEFINE_HPP

#define List_DEFINE(TYPE) List_Class(TYPE) List_functions(TYPE)

#define List_Class(TYPE) struct Link_##TYPE{ \
    Link_##TYPE *next, *prev; \
};\
\
struct List_##TYPE##_Node{ \
    Link_##TYPE link;   \
    TYPE val; \
};\
\
struct List_##TYPE##_Iterator{\
    Link_##TYPE *link;\
	TYPE (*deref)(List_##TYPE##_Iterator *);\
	void (*inc)(List_##TYPE##_Iterator *);\
	void (*dec)(List_##TYPE##_Iterator *);\
};\
\
struct List_##TYPE {\
	Link_##TYPE head;\
	int container_size;\
	char type[(int)sizeof "List_"#TYPE];\
	List_##TYPE##_Iterator (*push_front)(List_##TYPE * , TYPE);\
	List_##TYPE##_Iterator (*push_back)(List_##TYPE * , TYPE);\
	List_##TYPE##_Iterator (*begin)(List_##TYPE *);\
	List_##TYPE##_Iterator (*end)(List_##TYPE *);\
	void (*sort)(List_##TYPE *);\
	void (*erase)(List_##TYPE *, List_##TYPE##_Iterator);\
	void (*delet)(List_##TYPE *);\
};

#define List_functions(TYPE) void insert_before(Link_##TYPE *pos, List_##TYPE##_Node *n)\
{\
    n->link.next = pos;\
    n->link.prev = pos->prev;\
    pos->prev->next = &(n->link);\
    pos->prev = &(n->link);\
}\
\
List_##TYPE##_Node * List_##TYPE##_Node_new(const TYPE &val)\
{\
    List_##TYPE##_Node *n = (List_##TYPE##_Node *) malloc(sizeof (List_##TYPE##_Node));\
    n->link.next = n->link.prev = NULL;\
    n->val = val;\
    return n;\
}\
\
void List_##TYPE##_it_inc(List_##TYPE##_Iterator *it)\
{\
    	it->link = it->link->next;\
}\
\
void List_##TYPE##_it_dec(List_##TYPE##_Iterator *it)\
{\
    it->link = it->link->prev;\
}\
\
TYPE List_##TYPE##_it_deref(List_##TYPE##_Iterator *it)\
{\
    return ((List_##TYPE##_Node *)it->link)->val;\
}\
\
List_##TYPE##_Iterator List_##TYPE##_it_ctor(Link_##TYPE *l)\
{\
    List_##TYPE##_Iterator Iterator;\
    Iterator.deref = &List_##TYPE##_it_deref;\
    Iterator.inc = &List_##TYPE##_it_inc;\
    Iterator.dec = &List_##TYPE##_it_dec;\
    Iterator.link = l;\
    return Iterator;\
}\
\
List_##TYPE##_Iterator List_##TYPE##_begin(List_##TYPE * list)\
{\
	 return List_##TYPE##_it_ctor(list->head.next);\
}\
\
List_##TYPE##_Iterator List_##TYPE##_end(List_##TYPE * list)\
{\
	return List_##TYPE##_it_ctor(&(list->head));\
}\
\
List_##TYPE##_Iterator List_##TYPE##_push_front(List_##TYPE *list, TYPE p)\
{\
    List_##TYPE##_Node * pn1 = List_##TYPE##_Node_new(p);\
    insert_before(list->head.next, pn1);\
	list->container_size++;\
	return List_##TYPE##_it_ctor(list->head.next);\
}\
\
List_##TYPE##_Iterator List_##TYPE##_push_back(List_##TYPE *list, TYPE p)\
{\
    List_##TYPE##_Node * pn1 = List_##TYPE##_Node_new(p);\
    insert_before(&(list->head), pn1);\
	list->container_size++;\
	return List_##TYPE##_it_ctor(list->head.prev);\
}\
\
int List_##TYPE##_Iterator_equal(List_##TYPE##_Iterator it1, List_##TYPE##_Iterator it2)\
{\
	return it1.link == it2.link;\
}\
\
void List_##TYPE##_erase(List_##TYPE * list, List_##TYPE##_Iterator it)\
{\
	((List_##TYPE##_Node *)it.link)->link.next->prev = ((List_##TYPE##_Node *)it.link)->link.prev;\
    ((List_##TYPE##_Node *)it.link)->link.prev->next = ((List_##TYPE##_Node *)it.link)->link.next;\
    free((List_##TYPE##_Node *)it.link);\
    list->container_size--;\
}\
\
Link_##TYPE *merge_sort(Link_##TYPE *l)\
{\
    if(l->next == NULL)\
    {\
        return l;\
    }\
    else\
    {\
        Link_##TYPE *hl[2] = {NULL, NULL};\
        int i = 0;\
        Link_##TYPE *next;\
        for (Link_##TYPE *n = l; n != 0; n = next, i = i^1)\
        {\
          next = n->next;\
          n->next = hl[i];\
          hl[i] = n;\
        }\
		\
        /* Sort the halves.*/\
        hl[0] = merge_sort(hl[0]);\
        hl[1] = merge_sort(hl[1]);\
		\
        /* Do the merge.  Use a tail pointer pointer to simplify appending.*/\
        Link_##TYPE **tpp = &l;\
        while (hl[0] != NULL || hl[1] != NULL)\
        {\
            Link_##TYPE **pp = NULL;\
            /* Find the half that should be advanced.*/\
            for (int i = 0; i < 2; i++) {\
                /* p1 is one half and p2 is the other, alternately.*/\
                Link_##TYPE *&p1(hl[i]), *&p2(hl[i^1]);\
                if ( (p2 == 0 && p1 != 0) || (p1 != 0 && !(TYPE##_less(List_##TYPE##_it_deref((List_##TYPE##_Iterator *)&p2), List_##TYPE##_it_deref((List_##TYPE##_Iterator *)&p1))))){\
                    pp = &p1; /* Save the addr of pointer to the half*/\
                            /*that should be advanced.*/\
                }\
            }\
            *tpp = *pp; /* Append to the end of the merged list.*/\
			*pp = (*pp)->next; /* Advance the pointer to that half.*/\
            (*tpp)->next = 0; /* Terminate it.*/\
            tpp = &(*tpp)->next; /* Advance tail pointer pointer.*/\
        }\
		\
        return l;\
    }\
    /*return list_sorted;*/\
}\
\
void List_##TYPE##_sort(List_##TYPE * list)\
{\
	 if (list->container_size > 1){\
		/* breaking the circular list and setting last element pointing to null */\
		list->head.prev->next = NULL;\
		/* sort the list */\
		list->head.next = merge_sort(list->head.next);\
		/* fixing pointer to be a circular list back again */\
		Link_##TYPE *pos = list->head.next;	\
		Link_##TYPE *tmp = &list->head;	\
		while(pos != NULL)\
		{\
			pos->prev = tmp;\
			tmp = pos;\
			pos = pos->next;\
		}\
			/* fixing last element*/\
			tmp->next = &list->head;\
			/* fixing head prev pointer to point to last element;*/\
			list->head.prev = tmp;\
	}\
}\
\
void List_##TYPE##_delet(List_##TYPE * list)\
{\
	while(list->container_size != 0)\
		List_##TYPE##_erase(list, List_##TYPE##_begin(list));\
	free(list);\
}\
\
List_##TYPE * List_##TYPE##_new(bool (*f) (const TYPE &ob1, const TYPE &ob2))\
{\
		List_##TYPE *container = (List_##TYPE *)malloc(sizeof(List_##TYPE));\
		container->container_size = 0; \
		container->head.next = &(container->head);\
		container->head.prev = &(container->head);\
		container->push_front = &List_##TYPE##_push_front;\
		container->push_back = &List_##TYPE##_push_back;\
		strcpy(container->type, "List_"#TYPE);\
		container->begin = &List_##TYPE##_begin;\
		container->end = &List_##TYPE##_end;\
		container->erase = &List_##TYPE##_erase;\
		container->sort = &List_##TYPE##_sort;\
		container->delet = &List_##TYPE##_delet;\
		return container;\
	}

#endif 
