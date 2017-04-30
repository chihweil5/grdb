#ifndef __CONV_H
#define __CONV_H

#include "graph.h"
#include "stack.h"

struct attrBuf{
	attribute_t *attrbuf;
	int num;
};

typedef struct attrBuf *attrBuf_pt;

void convert_normal_form(graph_t g);
stack_pt get_neighbors(graph_t g, vertex_t target);
//schema_t find_largest_common_sv(graph_t g, stack_pt s, vertex_t target);


#endif