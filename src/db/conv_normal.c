#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "conv.h"


#define MAX 10



attrBuf_pt get_attribute(tuple_t t){

	attrBuf_pt attrList = (attrBuf_pt) malloc(sizeof(struct attrBuf));
	attrList -> attrbuf = (attribute_t *)malloc(sizeof(struct attribute) * MAX);
	attrList -> num = 0;
	
	attribute_t *tgtAttr = attrList -> attrbuf;
	attribute_t attr;
	
	int i = 0;

	for (attr = t->s->attrlist; attr != NULL; attr = attr->next) {
		int offset = tuple_get_offset(t, attr->name);
		if (offset >= 0) {
			switch (attr->bt) {
			case CHARACTER:
				if(tuple_get_char(t->buf + offset) != 0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case VARCHAR:
				if(strcmp((char *) (t->buf + offset), "") == 0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case BOOLEAN:
				if(tuple_get_bool(t->buf + offset) != 0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case ENUM:
				//if(tuple_print_enum(t, offset, el) != 0)
				//	tgtAttr[i] = attr;
				break;

			case INTEGER:
				if(tuple_get_int(t->buf + offset) != 0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case FLOAT:
				if(tuple_get_float(t->buf + offset) != 0.0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case DOUBLE:
				if(tuple_get_double(t->buf + offset) != 0.0){
					tgtAttr[i] = attr;
					i++;
				}
				break;

			case DATE:
				break;

			case TIME:
				break;

			case BASE_TYPES_MAX:
				break;
			}
			
		}
		
	}

	attrList->num = i;
	return attrList;
}

int find_common_schema(attrBuf_pt target1, attrBuf_pt target2){
	//printf("in find_common_schema... \n");
	int i,j,commonNum = 0;
	attribute_t *target1_attrList = target1 -> attrbuf;
	attribute_t *target2_attrList = target2 -> attrbuf; //attribute list of top vertex
	for(i = 0; i < target1->num; i++) {
		for(j = 0; j < target2->num; j++) {

			if(strcmp(target1_attrList[i]->name, target2_attrList[j]->name) == 0){
				commonNum++;
			}
			
		}
	}
	//printf("return number of common attribute:%d\n", commonNum);
	return commonNum;
}

void change_common_schema(attrBuf_pt target1, attrBuf_pt target2, schema_t *common_schema){
	//printf("in change_common_schema... \n");
	int i,j,commonNum = 0;
	attribute_t *target1_attrList = target1 -> attrbuf;
	attribute_t *target2_attrList = target2 -> attrbuf; //attribute list of top vertex
	attribute_t common_attr = NULL;
	for(i = 0; i < target1->num; i++) {
		for(j = 0; j < target2->num; j++) {

			if(strcmp(target1_attrList[i]->name, target2_attrList[j]->name) == 0){
				common_attr = target2_attrList[j];
				schema_attribute_insert(*common_schema, common_attr);
				commonNum++;
			}
			
		}
	}

	//printf("finished changing schema\n");
}

int has_common_schema(schema_t s, attrBuf_pt node_att){

	attribute_t t;
	attribute_t *attr2 = node_att -> attrbuf;
	int i, sCount, nCount;
	sCount = 0;
	nCount = 0;
	for(t = s->attrlist; t != NULL; t = t->next){
		//schema_attribute_print(t);
		//printf("\n");
		sCount++;
		for(i = 0; i< node_att->num; i++){
			if(strcmp(t->name, attr2[i]->name) == 0){
				nCount++;
			}
		}
	}

	if(sCount == nCount){
		return 1;
	}
	else{
		return 0;
	}
	
}

stack_pt get_neighbors(graph_t g, vertex_t target){
	stack_pt s = (stack_pt) malloc(sizeof(struct stack));
	s->s = (vertex_t *)malloc(sizeof(struct vertex) * MAX);
	s->top = -1;
	
	edge_t edge;
	vertex_t vertex;
	assert (g != NULL);

	for (edge = g->e; edge != NULL; edge = edge->next){
		//printf("edge = %llu <-> %llu\n", edge->id1, edge->id2 );

		if(edge->id1 == target->id){
			//printf("e: %llu <-> %llu\n", edge->id1, edge->id2);
			vertex = graph_find_vertex_by_id(g, edge->id2);
			//printf("vertex: %llu\n", vertex->id);
			if(!isExist(vertex, s)){
				push(vertex, s);
			}
		} 
		else if(edge->id2 == target->id){
			//printf("e: %llu <-> %llu\n", edge->id2, edge->id1);
			vertex = graph_find_vertex_by_id(g, edge->id1);
			if(!isExist(vertex, s))
				push(vertex, s);
		}
	}
	return s;
}

int find_largest_common_sv(graph_t cst, stack_pt s, vertex_t target, schema_t max_common_schema, attrBuf_pt tgtAttr){

	/*tuple_t t = target->tuple;
	attrBuf_pt tgtAttr = get_attribute(t);*/

	//attribute_t *target_attrList = tgtAttr -> attrbuf;
	//int i;
/*
	printf("in find_largest_common_sv funciton: attrbute number -> %d\n", tgtAttr->num);

	for(i = 0; i < tgtAttr->num; i++){
		printf("in find_largest_common_sv funciton: ");
		schema_attribute_print(target_attrList[i]);
		printf("\n");
	}
*/
	//get neighbor nodes

	vertex_t top_vertex = peek(s);
	int v_large_schema = top_vertex->id;
	pop(s);
	attrBuf_pt top_att = get_attribute(top_vertex->tuple);


	int count = find_common_schema(tgtAttr, top_att);
	change_common_schema(tgtAttr, top_att, &max_common_schema);
	//printf("in find_largest_common_sv funciton: get common schema number:%d\n", count);

	//schema_print(max_common_schema);

	while(!isempty(s)){
		vertex_t tmp_vertex = peek(s);
		pop(s);
		attrBuf_pt tmp_att = get_attribute(tmp_vertex->tuple);
		int tmp_count = find_common_schema(tgtAttr, tmp_att);
		if(tmp_count > count) {
			change_common_schema(tgtAttr, tmp_att, &max_common_schema);
			v_large_schema = tmp_vertex->id;
		}
	}
	/*printf("in find_largest_common_sv funciton: ");
	schema_print(max_common_schema);
	printf("\n");*/
	return v_large_schema;
	
}

void cst_grow(graph_t g, schema_t common_schema, graph_t cst){
	/*graph_print(g, 0);
	printf("check common schema:\n");
	schema_print(common_schema);
	printf("\n");*/

	edge_t edge;
	vertex_t node2;
	vertex_t root = cst->v;
	stack_pt s = (stack_pt) malloc(sizeof(struct stack));
	s->s = (vertex_t *)malloc(sizeof(struct vertex) * MAX);
	s->top = -1;
	push(root, s);

	while(!isempty(s)){
		vertex_t node = peek(s);
		//printf("stack[%d] has id: %llu nodes\n",s->top, node->id);
		pop(s);
		//graph_print(g, 0);
		//find neighbors with large common schema
		for (edge = g->e; edge != NULL; edge = edge->next){
			if(edge->id1 == node->id){
				/*printf("\nid: %llu\n", edge->id2);
				graph_print(g, 0);
				printf("\n");*/
				node2 = graph_find_vertex_by_id(g, edge->id2);
				attrBuf_pt node_att = get_attribute(node2->tuple);
				int hasCommon = has_common_schema(common_schema, node_att);
				//printf("has Common schema :%d\n", hasCommon);

				//find the node that has common schema
				if(hasCommon && !isExist(node2, s)){
					push(node2, s);

					/*create new vertex and edge and add to cst graph*/
					vertex_t v;
					v = (vertex_t) malloc(sizeof(struct vertex));
				   	assert (v != NULL);
				   	vertex_init(v);
				   	v->id = node2->id;
					graph_insert_vertex(cst, v);
					edge_t e = (edge_t) malloc(sizeof(struct edge));
					edge_init(e);
					edge_set_vertices(e, node->id, node2->id);
					graph_insert_edge(cst, e);
				}
			}
		}
		//printf("stack has %d nodes\n", s->top+1 );
	}

	/*remove attribute from schema*/
	attribute_t attr;
	for (attr = common_schema->attrlist; attr != NULL; attr = attr->next) {
		attribute_t attr_remove = schema_find_attr_by_name(g->sv, attr->name);
		schema_attribute_remove(g->sv, attr_remove);
	}

	/*printf("removing attributes after growing cst\n");
	schema_print(g->sv);
	printf("\n");*/
}


void convert_normal_form(graph_t g){
	vertex_t v = g->v;
	stack_pt stk = get_neighbors(g, v);

	/*vertex_t *stack = stk->s;
	int i;
	for ( i = 0; i <= stk->top; i++ ) {
      printf( "stack[%d] : %llu\n", i, stack[i]->id);
    }*/

    // one cst
	graph_t cst = (graph_t) malloc(sizeof(struct graph));

	schema_t max_common_schema = NULL;
	schema_init(&max_common_schema);
	max_common_schema->attrlist = NULL;

	tuple_t t = v->tuple;
	attrBuf_pt tgtAttr = get_attribute(t);

    int vid_large_schema = find_largest_common_sv(cst, stk, v, max_common_schema, tgtAttr);
    
	//set up cst graph
	vertex_t newv;
	newv = (vertex_t) malloc(sizeof(struct vertex));
   	assert (newv != NULL);
   	vertex_init(newv);
   	newv->id = v->id;
   	graph_insert_vertex(cst, newv);

   	edge_t e = (edge_t) malloc(sizeof(struct edge));
	edge_init(e);
	edge_set_vertices(e, newv->id, vid_large_schema);

	cst->sv = max_common_schema;

    cst_grow(g, max_common_schema, cst);


    printf("normal form:\n");
    graph_print(cst, 0);

    printf("\n");
	

	/*
    graph_t *cst = (graph_t *) malloc(sizeof(struct graph) * MAX);
    for(i = 0; i < MAX; i++){
    	cst[i] = (graph_t) malloc(sizeof(struct graph));
    	graph_init(cst[i]);
    }

    int cstNum = 0;
    vertex_t v_tmp;
    int vid_large_schema = -1;
    
    while(g->sv != NULL){
    	printf("graph schema: ");
    	schema_print(g->sv);
    	printf("\n");
    	schema_t max_common_schema = NULL;
		schema_init(&max_common_schema);
		max_common_schema->attrlist = NULL;

		//get vertex attribute
		//for (v_tmp = g->v; v_tmp != NULL; v_tmp = v_tmp->next) {
			tuple_t t = v->tuple;
			attrBuf_pt tgtAttr = get_attribute(t);
			if(tgtAttr->num > 0){
				vid_large_schema = find_largest_common_sv(cst[cstNum], stk, v, max_common_schema, tgtAttr);
			}
		//}
		
		//set up cst graph
		vertex_t newv;
		newv = (vertex_t) malloc(sizeof(struct vertex));
	   	assert (newv != NULL);
	   	vertex_init(newv);
	   	newv->id = v->id;
	   	graph_insert_vertex(cst[cstNum], newv);

	   	edge_t e = (edge_t) malloc(sizeof(struct edge));
		edge_init(e);
		edge_set_vertices(e, newv->id, vid_large_schema);

		cst[cstNum]->sv = max_common_schema;
		//graph_print(cst[cstNum], 0);

	    cst_grow(g, max_common_schema, cst[cstNum]);
	    printf("normal form:\n");
	    graph_print(cst[cstNum], 0);

	    printf("\n");
	    cstNum++;
    }*/
    
}




