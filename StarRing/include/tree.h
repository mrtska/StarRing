/*
[Module tree.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/28
*/

#pragma once

#include <list.h>
#include <string.h>
#include <slab.h>

struct tree_node {

	void *value;
	struct list_head children;
	struct tree_node *parent;
};


struct tree {

	size_t nodes;
	struct tree_node *root;
};





static __inline__ struct tree *tree_create(void) {

	struct tree *out = kmalloc(sizeof(struct tree), 0);
	out->nodes = 0;
	out->root = NULL;
	return out;
}

static __inline__ struct tree_node *tree_node_create(void *value) {

	struct tree_node *node = kmalloc(sizeof(struct tree_node), 0);
	node->value = value;
	INIT_LIST_HEAD(&node->children);
	node->parent = NULL;
	return node;
}

static __inline__ void tree_set_root(struct tree *tree, void *value) {

	struct tree_node *root = tree_node_create(value);
	tree->root = root;
	tree->nodes = 1;
}





















