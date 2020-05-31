#include "bp-search.h"

#define DEBUG true
#define debug_print(fmt, ...)            \
  do                                     \
  {                                      \
    if (DEBUG)                           \
      fprintf(stdout, fmt, __VA_ARGS__); \
    fflush(stdout);                      \
  } while (0)

// 航班信息结构
typedef struct PlaneInfo
{
  char plane_num[MAX_LEN];
  char fly_num[MAX_LEN];
  char setoff_time[MAX_LEN];
  char start_station[MAX_LEN];
  char terminal_station[MAX_LEN];
  char fly_duration[MAX_LEN];
  int passenger_limit;
  int ticket_left;
} PlaneInfo;

// bplustree节点，包含阶数以及指向根节点的指针
typedef struct BPTree
{
  int level;
  struct BPTreeNode *root;
} BPTree;

// 如果不是叶子节点就包含一个指向子结点的指针
typedef struct BPTreeEntry
{
  // 节点唯一标识值
  // 可以用来判断节点在树中的位置
  char key[KEY_MAX_LEN];
  union {
    struct BPTreeNode *child;
    struct PlaneInfo *value;
  };
} BPTreeEntry;

// bplustree节点
typedef struct BPTreeNode
{
  int size;
  // 标记是否是叶子节点
  bool is_leaf;
  // 指向父节点的指针
  struct BPTreeNode *parent;
  struct BPTreeEntry entries[FANOUT + 1];
} BPTreeNode;

// 字符串连接
char *key_cat(char fly_num[MAX_LEN], char setoff_time[MAX_LEN])
{
  static char new_key[KEY_MAX_LEN];
  int i = 0;
  int j = 0;
  for (i; i < KEY_MAX_LEN; i++)
  {
    new_key[i] = '\0';
  }
  for (i = 0; i < MAX_LEN && fly_num[i] != '\0'; i++)
  {
    new_key[i] = fly_num[i];
  }
  for (i; i < KEY_MAX_LEN && setoff_time[j] != '\0';)
  {
    new_key[i] = setoff_time[j];
    i++;
    j++;
  }

  return new_key;
}

// 字符串复制
void key_copy(char key1[KEY_MAX_LEN], char key[KEY_MAX_LEN])
{
  int i = 0;
  for (i = 0; i < KEY_MAX_LEN; i++)
  {
    key1[i] = '\0';
  }
  for (i = 0; i < KEY_MAX_LEN && key[i] != '\0'; i++)
  {
    key1[i] = key[i];
  }
}
void val_copy(char val1[MAX_LEN], char val[MAX_LEN])
{
  int i = 0;
  for (; i < MAX_LEN; i++)
  {
    val1[i] = '\0';
  }
  i = 0;
  for (; i < MAX_LEN && val[i] != '\0'; i++)
  {

    val1[i] = val[i];
  }
}

// 字符串比较
int key_cmp(char key1[KEY_MAX_LEN], char key[KEY_MAX_LEN])
{
  int i = 0;
  for (i; i < KEY_MAX_LEN; i++)
  {
    if (key1[i] > key[i] || (key1[i] != '\0' && key[i] == '\0'))
    {
      return 1;
    }
    else if (key1[i] < key[i] || (key1[i] == '\0' && key[i] != '\0'))
    {
      return -1;
    }
    else if (key1[i] == '\0' && key[i] == '\0')
    {
      break;
    }
  }
  return 0;
}

// 输出节点的key
void __print_node(struct BPTreeNode *node)
{
  printf("\n\n\t始发站                  终点站                  航班号          起飞时间             飞机号             飞行时间              成员定额       票余量\n");
  printf("    ----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  // 如果node为叶子节点就将i赋值为0，否则赋值为1
  int i = node->is_leaf ? 0 : 1;
  PlaneInfo *pi;
  for (; i < node->size; i++)
  {
    pi = node->entries[i].value;
    printf("\t%s       |  %s       |  %s        |  %s     |  %s      |  %s         |  %d         |  %d\n",
           &pi->start_station, &pi->terminal_station, &pi->fly_num, &pi->setoff_time, &pi->plane_num, &pi->fly_duration, pi->passenger_limit, pi->ticket_left);
    printf("    ----------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");
  }
  printf("\n\n");
}

// 输出bplustree
void __print__bptree(struct BPTreeNode *root)
{
  // 如果是叶子节点就输出
  if (root->is_leaf)
  {
    __print_node(root);
  }
  else
  {
    // 输出当前节点
    __print_node(root);
    // 递归寻找叶子节点
    int i = 0;
    for (; i < root->size; i++)
    {
      __print__bptree(root->entries[i].child);
    }
  }
}

static int _bptree_node_find_pos(struct BPTreeNode *const node, char key[KEY_MAX_LEN])
{
  // 初识返回值为0
  // 作为位置标记
  int ret = 0;
  // 如果是叶子节点
  if (node->is_leaf)
  {
    // 如果节点的size为0就返回0
    if (node->size == 0)
    {
      return 0;
    }

    // 否则遍历从索引为0的叶子节点，比较key值
    // 找到一个arr[i] < k <= arr[i+1]的位置
    // 查找范围是node->size
    // 每次循环递增返回值
    while (key_cmp(key, node->entries[ret].key) > 0 && ret < node->size)
    {
      ret++;
    }
  }
  // 否则
  else
  {
    // 修改返回值为1
    ret = 1;
    // 遍历同级节点
    // 找到一个arr[i] < k < arr[i+1]的位置
    // 每次循环递增返回值
    // 查找范围是node->size
    while (key_cmp(key, node->entries[ret].key) > 0 && ret < node->size)
    {
      ret++;
    }
  }
  // 返回
  return ret;
}

// 插入节点
static int _bptree_node_insert(struct BPTreeNode *node, struct BPTreeEntry *entry)
{
  // 找到在同一级中的合适的插入位置
  int index = _bptree_node_find_pos(node, entry->key);
  // 如果位置的索引在size范围之内
  // 就对同级节点做移动操作
  // 否则不用操作
  if (index < node->size)
  {
    memmove(&node->entries[index + 1], &node->entries[index],
            (node->size - index) * sizeof(struct BPTreeEntry));
  }
  node->entries[index] = *entry;
  // 将该点插入
  node->entries[index] = *entry;
  // 如果不是叶子节点
  if (!node->is_leaf)
    entry->child->parent = node;
  // 增加size
  node->size++;
  return 0;
}

// 插入叶子节点
static int _bptree_leaf_node_insert(struct BPTreeNode *node, char key[KEY_MAX_LEN],
                                    struct PlaneInfo *value)
{
  char key1[KEY_MAX_LEN];
  key_copy(key1, key);
  PlaneInfo *value1 = (PlaneInfo *)malloc(sizeof(PlaneInfo));
  val_copy(value1->plane_num, value->plane_num);
  val_copy(value1->fly_num, value->fly_num);
  val_copy(value1->setoff_time, value->setoff_time);
  val_copy(value1->start_station, value->start_station);
  val_copy(value1->terminal_station, value->terminal_station);
  val_copy(value1->fly_duration, value->fly_duration);
  value1->passenger_limit = value->passenger_limit;
  value1->ticket_left = value->ticket_left;
  BPTreeEntry *entry = (BPTreeEntry *)malloc(sizeof(struct BPTreeEntry));
  key_copy(entry->key, key1);
  entry->value = value1;
  return _bptree_node_insert(node, entry);
}

// 插入内部节点
static int _bptree_internal_node_insert(struct BPTreeNode *node, char key[KEY_MAX_LEN],
                                        struct BPTreeNode *child)
{
  printf(" _bptree_internal_node_insert_start\n");
  char key1[KEY_MAX_LEN];
  key_copy(key1, key);
  BPTreeEntry *entry = (BPTreeEntry *)malloc(sizeof(struct BPTreeEntry));
  key_copy(entry->key, key1);
  entry->child = child;
  printf(" _bptree_internal_node_insert_end\n");
  return _bptree_node_insert(node, entry);
}

// 更新节点的父节点
static void _bptree_update_child_parent(struct BPTreeNode *const node)
{
  // 将同级节点中前size个节点的子结点指向node
  int i = 0;
  for (; i < node->size; i++)
  {
    node->entries[i].child->parent = node;
  }
}

// 分裂
static int _bptree_split(struct BPTree *const tree, struct BPTreeNode *node)
{
  // 分配左右两边的节点个数
  printf("_bptree_split_start\n");
  int left_size = node->size / 2;
  int right_size = node->size - left_size;
  struct BPTreeNode *right_node = (struct BPTreeNode *)
      calloc(1, sizeof(struct BPTreeNode));

  node->size = left_size;
  memmove(&right_node->entries, &node->entries[left_size],
          right_size * sizeof(struct BPTreeEntry));
  right_node->size = right_size;
  right_node->parent = node->parent;
  right_node->is_leaf = node->is_leaf;

  char *pivot;
  key_copy(pivot, right_node->entries[0].key);
  printf("pivot: %s\n", pivot);

  if (node->parent)
  {
    _bptree_internal_node_insert(node->parent, pivot, right_node);
  }
  else
  {
    tree->level++;
    struct BPTreeNode *parent = (struct BPTreeNode *)
        calloc(1, sizeof(struct BPTreeNode));
    BPTreeEntry nullentry;
    key_copy(nullentry.key, '\0');
    nullentry.child = node;
    parent->entries[0] = nullentry;
    parent->size = 1;
    node->parent = parent;
    _bptree_internal_node_insert(parent, pivot, right_node);
    tree->root = parent;
  }
  if (!node->is_leaf)
  {
    _bptree_update_child_parent(node);
    _bptree_update_child_parent(right_node);
  }
  struct BPTreeNode *parent = node->parent;
  while (parent && parent->size >= FANOUT + 1)
  {
    _bptree_split(tree, parent);
    parent = parent->parent;
  }
  printf("_bptree_split_end\n");
  return 0;
}

// 创建bplustree
struct BPTree *bptree_create(void)
{
  struct BPTree *tree = (struct BPTree *)malloc(sizeof(struct BPTree));
  tree->level = 0;
  tree->root = (struct BPTreeNode *)calloc(1, sizeof(struct BPTreeNode));
  tree->root->is_leaf = true;
  return tree;
}

// 插入叶子节点
int bptree_insert(struct BPTree *const tree, char key[KEY_MAX_LEN], struct PlaneInfo *value)
{
  struct BPTreeNode *node = tree->root;
  // 遍历找到适合的叶子节点位置
  while (!node->is_leaf)
  {
    node = node->entries[_bptree_node_find_pos(node, key) - 1].child;
  }
  // 插入叶子节点
  _bptree_leaf_node_insert(node, key, value);
  // 如果节点的个数超过最大值FANOUT，就要进行分裂
  if (node->size == FANOUT + 1)
  {
    _bptree_split(tree, node);
  }
  return 0;
}

// 查找
struct PlaneInfo *bptree_lookup(struct BPTree *const tree, char key[KEY_MAX_LEN])
{
  char key1[KEY_MAX_LEN];
  key_copy(key1, key);
  // 从根节点开始找
  struct BPTreeNode *node = tree->root;
  // 如果节点不是叶子节点
  // 就找到合适的叶子节点
  while (!node->is_leaf)
  {
    node = node->entries[_bptree_node_find_pos(node, key1) - 1].child;
  }
  // 找出叶子节点的索引
  int index = _bptree_node_find_pos(node, key1);
  // 如果在范围之内，就将叶子节点的值返回出去
  if (index < node->size)
  {
    //    	printf("bptree_lookup_end\n");
    if (index == 0 && key_cmp(node->entries[index].key, key1) != 0)
    {
      return NULL;
    }
    return node->entries[index].value;
  }
}

// 删除
int bptree_delete(struct BPTree *const tree, char key[KEY_MAX_LEN])
{
  char key1[KEY_MAX_LEN];
  key_copy(key1, key);
  // 从根节点开始找
  struct BPTreeNode *node = tree->root;
  // 如果不是叶子节点
  // 就找到一个合适的叶子节点
  while (!node->is_leaf)
  {
    node = node->entries[_bptree_node_find_pos(node, key1) - 1].child;
  }
  // 找出叶子节点的索引值
  int index = _bptree_node_find_pos(node, key1);
  // 如果在范围之内
  if (index < node->size)
  {
    // 对同级节点在数组中的位置做移动
    memmove(&node->entries[index], &node->entries[index + 1],
            (node->size - index) * sizeof(struct BPTreeEntry));
    node->size--;
  }
  return 0;
}

// 销毁节点
int _bptree_destroy_node_rec(struct BPTreeNode *const node)
{
  // 如果节点不是叶子节点
  // 就做递归的删除
  if (!node->is_leaf)
  {
    int i = 0;
    for (; i < node->size; i++)
    {
      _bptree_destroy_node_rec(node->entries[i].child);
    }
  }
  // 删除节点
  free(node);
  return 0;
}

// 销毁树
int bptree_destroy(struct BPTree *const tree)
{
  // 递归删除所有节点
  int ret = _bptree_destroy_node_rec(tree->root);
  free(tree);
  return ret;
}

// 读取文件中的航班信息从而添加节点
void bptree_fill(struct BPTree *const tree)
{
  // 逐行读取文件
  // 每读一行添加一个叶子节点
  FILE *fp = fopen("index.txt", "r");
  if (fp == NULL)
    return;
  char plane_num[MAX_LEN];
  char fly_num[MAX_LEN];
  char setoff_time[MAX_LEN];
  char start_station[MAX_LEN];
  char terminal_station[MAX_LEN];
  char fly_duration[MAX_LEN];
  int passenger_limit = 0;
  int ticket_left = 0;
  int i = 0;

  while (!feof(fp))
  {
    i = 0;
    char key[KEY_MAX_LEN];
    PlaneInfo *plane_info = (PlaneInfo *)malloc(sizeof(struct PlaneInfo));

    for (i = 0; i < MAX_LEN; i++)
    {
      plane_num[i] = '\0';
      fly_num[i] = '\0';
      setoff_time[i] = '\0';
      start_station[i] = '\0';
      terminal_station[i] = '\0';
      fly_duration[i] = '\0';
    }

    // 读取数据
    fscanf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\n", &plane_num, &fly_num,
           &setoff_time, &start_station, &terminal_station, &fly_duration, &passenger_limit, &ticket_left);
    // 使用数据初始化信息

    val_copy(plane_info->plane_num, plane_num);
    val_copy(plane_info->fly_num, fly_num);
    val_copy(plane_info->setoff_time, setoff_time);
    val_copy(plane_info->start_station, start_station);
    val_copy(plane_info->terminal_station, terminal_station);
    val_copy(plane_info->fly_duration, fly_duration);
    plane_info->passenger_limit = passenger_limit;
    plane_info->ticket_left = ticket_left;
    key_copy(key, key_cat(plane_info->fly_num, plane_info->setoff_time));

    bptree_insert(tree, key, plane_info);
    free(plane_info);
  }
  fclose(fp);
}

struct BPTree *bptree_init()
{
  struct BPTree *tree = bptree_create();
  bptree_fill(tree);
  return tree;
}

void bptree_search(struct BPTree *const tree)
{
  char fly_num[MAX_LEN];
  char setoff_time[MAX_LEN];
  char fly_num_temp[MAX_LEN];
  char setoff_time_temp[MAX_LEN];
  char key[KEY_MAX_LEN];
  struct PlaneInfo *pi;
  int i = 0;

  printf("\n   ****************************************\n\n");
  printf("     请输入您要查询的航班号： ");
  scanf("%s", &fly_num_temp);
  printf("\n     请输入您要查询的起飞时间： ");
  scanf("%s", &setoff_time_temp);
  printf("\n   ****************************************\n\n");
  for (i = 0; i < MAX_LEN; i++)
  {
    fly_num[i] = '\0';
    setoff_time[i] = '\0';
  }

  val_copy(fly_num, fly_num_temp);
  val_copy(setoff_time, setoff_time_temp);
  key_copy(key, key_cat(fly_num, setoff_time));

  pi = bptree_lookup(tree, key);

  if (pi)
  {
    printf("\n   搜索结果\n\n");
    printf("\n\t航班号        起飞时间             始发站                  终点站                  飞机号              飞行时间               成员定额       票余量\n");
    printf("    ----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("\t%s     |  %s      |  %s       |  %s       |  %s        | %s         |  %d         |  %d\n",
           &pi->fly_num, &pi->setoff_time, &pi->start_station, &pi->terminal_station, &pi->plane_num, &pi->fly_duration, pi->passenger_limit, pi->ticket_left);
    printf("    ----------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");
  }
  else
  {
    printf("\n\t抱歉，没有匹配的结果\n\n\n");
  }
}

void use_bptree_search(struct BPTree *const tree)
{
  int choice = 3;
  int flag = 1;

  while (flag)
  {
    printf("\n   ****************************************\n\n");
    printf("     1. 根据航班号和起飞时间查询航班信息\n\n");
    printf("     2. 查看所有航班信息\n\n");
    printf("     3. 退出航班索引功能\n");
    printf("\n   ****************************************\n\n");
    printf("     请选择您需要的服务： ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
      bptree_search(tree);
      break;
    case 2:
      __print__bptree(tree->root);
      break;
    case 3:
      flag = 0;
      break;
    }
  }
}
