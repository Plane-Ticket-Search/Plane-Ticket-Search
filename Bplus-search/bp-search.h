/*
 * An in-memory naive implementation of B+-Tree
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#define FANOUT 32
#define MAX_LEN 30
#define KEY_MAX_LEN 60

struct PlaneInfo;
struct BPTree;
struct BPTreeEntry;
struct BPTreeNode;

extern struct BPTree *bptree_create(void);
extern int bptree_insert(struct BPTree *const, char key[KEY_MAX_LEN], struct PlaneInfo *);
extern int bptree_delete(struct BPTree *const, char key[KEY_MAX_LEN]);
extern int bptree_destroy(struct BPTree *);
extern void bptree_fill(struct BPTree *const tree);
extern void __print_node(struct BPTreeNode *node);
extern void __print__bptree(struct BPTreeNode *root);
static int _bptree_node_find_pos(struct BPTreeNode *node, char key[KEY_MAX_LEN]);
static int _bptree_node_insert(struct BPTreeNode *node, struct BPTreeEntry *entry);
static int _bptree_leaf_node_insert(struct BPTreeNode *node, char key[KEY_MAX_LEN], struct PlaneInfo *value);
static int _bptree_internal_node_insert(struct BPTreeNode *node, char key[KEY_MAX_LEN], struct BPTreeNode *child);
static void _bptree_update_child_parent(struct BPTreeNode *node);
static int _bptree_split(struct BPTree *const tree, struct BPTreeNode *node);
extern struct PlaneInfo *bptree_lookup(struct BPTree *const tree, char key[KEY_MAX_LEN]);
extern int _bptree_destroy_node_rec(struct BPTreeNode *node);
extern char *key_cat(char fly_num[MAX_LEN], char setoff_time[MAX_LEN]);
extern void key_copy(char key1[KEY_MAX_LEN], char key[KEY_MAX_LEN]);
extern void val_copy(char val1[MAX_LEN], char val[MAX_LEN]);
extern int key_cmp(char key1[KEY_MAX_LEN], char key[KEY_MAX_LEN]);
extern struct BPTree *bptree_init();
extern void bptree_search(struct BPTree *const tree);
extern void insert_data(struct BPTree *const tree);
extern void use_bptree_search(struct BPTree *const tree);
