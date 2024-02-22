#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Btree with an order of 6
// m(order) = 6
// k (children) = m/2 -> m
// keys = k-1

#define TREE_ORDER 6
#define MAX_CHILDREN TREE_ORDER
#define MAX_KEYS MAX_CHILDREN - 1
#define MIN_CHILDREN TREE_ORDER / 2
#define MIN_KEYS (MAX_KEYS) / 2

struct Tree;

typedef struct Node
{
  int *keys;
  struct Node **children;
  struct Node *parent;
  size_t children_size;
  size_t keys_size;
  struct Tree *tree;
} Node;

typedef struct Tree
{
  Node *root;
} Tree;

typedef struct SearchResult
{
  int *key;
  size_t index;
  Node *node;
} SearchResult;

static int node_insert(Node *, const int, const int);

static inline int is_root_node(Node *node)
{
  return !node->parent;
}

static inline int is_leaf_node(Node *node)
{
  return !node->children_size;
}

static inline int is_inner_node(Node *node)
{
  return !(is_leaf_node(node) || is_root_node(node));
}

static inline int node_overflowed(Node *node)
{
  if (is_leaf_node(node))
    return node->keys_size > MAX_KEYS;
  return (node->keys_size > (node->children_size - 1)) || (node->children_size > MAX_CHILDREN);
}

static inline size_t min_node_keys(Node *node)
{
  if (is_leaf_node(node))
    return MIN_KEYS;
  return (node->children_size - 1) / 2;
}

static inline int node_deficient(Node *node)
{
  return node->keys_size < min_node_keys(node);
}

static inline int node_sufficient(Node *node)
{
  return node->keys_size > min_node_keys(node);
}

Node *
init_node(Tree *tree)
{
  Node *node = (Node *)malloc(sizeof(Node));
  if (!node)
    return 0;
  node->keys = (int *)malloc(sizeof(int[MAX_CHILDREN]));
  node->children = (Node **)malloc(sizeof(Node *) * MAX_CHILDREN + 1);
  for (size_t i = 0; i <= MAX_CHILDREN; i++)
    node->children[i] = (Node *)malloc(sizeof(Node *));

  if (!(node->keys && node->children))
  {
    free(node);
    return 0;
  }

  node->tree = tree;
  node->keys_size = 0;
  node->children_size = 0;
  return node;
}

size_t node_index(Node *node)
{
  if (is_root_node(node))
    return 0;
  for (size_t i = 0; i < node->parent->children_size; i++)
  {
    if (node == node->parent->children[i])
      return i;
  }
  return node->parent->parent->parent->keys[0];
}

static inline void split_node(Node *node)
{
  Node *parent = node->parent;
  if (!parent)
  {
    parent = init_node(node->tree);
    node->parent = parent;
    parent->children[0] = node;
    parent->children_size += 1;
    parent->tree->root = parent;
  }
  size_t const half_node_keys_size = node->keys_size / 2;
  Node *new_child = init_node(node->tree);

  memcpy(new_child->keys, node->keys + half_node_keys_size + 1, sizeof(int) * (node->keys_size - half_node_keys_size - 1));
  new_child->keys_size = node->keys_size - half_node_keys_size - 1;
  new_child->parent = parent;

  if (parent->children_size == node_index(node) + 1)
  {
    parent->children[parent->children_size] = new_child;
  }
  else
  {
    Node **destination = &parent->children[node_index(node) + 2];
    Node **source = &parent->children[node_index(node) + 1];
    size_t size = parent->children_size - node_index(node) - 1;
    memmove(destination, source, size * sizeof(Node *));
    parent->children[node_index(node) + 1] = new_child;
  }

  parent->children_size += 1;
  node->keys_size = half_node_keys_size;

  // promotion:
  node_insert(parent, node->keys[half_node_keys_size], 1);

  if (node->children_size)
  {
    Node **destination = new_child->children;
    Node **source = node->children + node->keys_size + 1;
    size_t size = new_child->keys_size + 1;

    memcpy(destination, source, size * sizeof(Node *));

    new_child->children_size = size;
    node->children_size = node->keys_size + 1;
    for (size_t i = 0; i < new_child->children_size; i++)
    {
      new_child->children[i]->parent = new_child;
    }
  }
}

static inline void index_insert(Node *node, size_t index, int key)
{
  if (node->keys_size && (index != node->keys_size))
  {
    memmove(node->keys + index + 1, node->keys + index, sizeof(int[node->keys_size - index]));
  }
  node->keys[index] = key;
  node->keys_size++;
}

static inline void index_remove(Node *node, size_t index)
{
  if (node->keys_size == index + 1)
  {
    node->keys_size--;
    return;
  }
  memmove(node->keys + index, node->keys + index + 1, sizeof(int) * (node->keys_size - index - 1));
  node->keys_size--;
}

static int node_insert(Node *node, int key, const int promotion)
{

  if (is_leaf_node(node) || promotion)
  {
    if (!node->keys_size)
    {
      index_insert(node, 0, key);
      return key;
    }
    if (key > node->keys[node->keys_size - 1])
    {
      index_insert(node, node->keys_size, key);
    }
    else
    {
      for (size_t i = 0; i < node->keys_size; i++)
      {
        if (key == node->keys[i])
          return 0;
        if (key < node->keys[i])
        {
          index_insert(node, i, key);
          break;
        }
      }
    }
  }
  else
  {
    int *last_key = node->keys + node->keys_size - 1;
    if (key > *last_key)
      return node_insert(node->children[node->children_size - 1], key, 0);

    size_t i = 0;

    while (i < node->keys_size)
    {
      if (key == node->keys[i])
        return 0;

      if (key < node->keys[i])
      {
        return node_insert(node->children[i], key, 0);
        break;
      }
      i++;
    }
  }

  if (node_overflowed(node))
    split_node(node);
  return key;
}

SearchResult *search(Tree *tree, int key)
{
  SearchResult *search_result = (SearchResult *)malloc(sizeof(SearchResult *));
  Node *node = tree->root;

  while (node->keys_size)
  {
    int *last_key = node->keys + node->keys_size - 1;
    if (key == *last_key)
    {
      printf("Searching: %i, in leaf?: %i\n", *last_key, is_leaf_node(node));
      *search_result = (SearchResult){last_key, node->keys_size - 1, node};
      return search_result;
    }
    if (key > *last_key)
    {
      printf("Searching: %i, in leaf?: %i\n", *last_key, is_leaf_node(node));
      if (is_leaf_node(node))
        return 0;
      node = node->children[node->keys_size];
      continue;
    }

    size_t i = 0;

    while (i < node->keys_size && key > node->keys[i])
    {
      printf("Searching: %i, in leaf?: %i\n", node->keys[i], is_leaf_node(node));
      i++;
    }

    if (key == node->keys[i])
    {
      *search_result = (SearchResult){node->keys + i, i, node};
      return search_result;
    }
    if (is_leaf_node(node))
      return 0;
    node = node->children[i];
  }
  return 0;
}

static inline void merge(Node *left_node, Node *right_node)
{
  Node *parent = left_node->parent;
  if (!parent)
    return;
  size_t left_index = node_index(left_node);
  int *separator = parent->keys + left_index;
  index_insert(left_node, left_node->keys_size, *separator);

  memcpy(left_node->keys + left_node->keys_size, right_node->keys, sizeof(int) * right_node->keys_size);
  left_node->keys_size += right_node->keys_size;
  if (!is_leaf_node(right_node))
  {
    memcpy(left_node->children + left_node->children_size, right_node->children, sizeof(Node *) * right_node->children_size);
    for (size_t i = 0; i < right_node->children_size; i++)
      right_node->children[i]->parent = left_node;
    left_node->children_size += right_node->children_size;
  }

  size_t right_index = left_index + 1;

  if ((right_index + 1) < parent->children_size)
  {
    memmove(&right_node, &parent->children[right_index + 1], sizeof(Node *) * (parent->children_size - left_index));
  }
  parent->children_size--;
  index_remove(parent, left_index);

  if (parent->children_size == 1)
  {
    left_node->tree->root = left_node;
  }
}

static inline void rebalance(Node *node)
{
  while (!is_root_node(node) && node_deficient(node))
  {
    size_t n_index = node_index(node);

    Node *left_sibling = NULL;
    Node *right_sibling = NULL;

    if (n_index < node->parent->children_size - 1)
    {
      right_sibling = node->parent->children[n_index + 1];
    }
    else
    {
      left_sibling = node->parent->children[n_index - 1];
    }

    if (right_sibling && node_sufficient(right_sibling))
    {
      int *separator = node->parent->keys + n_index;
      printf("Da seppppp is: %i\n", *separator);
      index_insert(node, node->keys_size, *separator);
      *separator = right_sibling->keys[0];
      index_remove(right_sibling, 0);
    }
    else if (left_sibling && node_sufficient(left_sibling))
    {
      int *separator = node->parent->keys + n_index - 1;
      printf("Da seppppp is: %i\n", *separator);
      index_insert(node, 0, *separator);
      *separator = left_sibling->keys[left_sibling->keys_size - 1];
      index_remove(left_sibling, left_sibling->keys_size - 1);
    }
    else
    {
      if (right_sibling)
      {
        merge(node, right_sibling);
      }
      else
      {
        merge(left_sibling, node);
      }
    }
  }
}

int delete(Tree *tree, int key)
{
  SearchResult *search_result = search(tree, key);
  if (!search_result)
    return -1;
  Node *node = search_result->node;
  size_t index = search_result->index;

  while (!is_leaf_node(node))
  {
    Node *left_child_node = node->children[index];
    Node *right_child_node = node->children[index + 1];
    if (node_sufficient(left_child_node))
    {
      size_t child_key_index = left_child_node->keys_size - 1;
      node->keys[index] = left_child_node->keys[child_key_index];
      node = left_child_node;
      index = child_key_index;
    }
    else if (node_sufficient(right_child_node))
    {
      node->keys[index] = right_child_node->keys[0];
      node = right_child_node;
      index = 0;
    }
  }

  index_remove(node, index);

  if (is_root_node(node))
    return 0;

  rebalance(node);

  return 0;
}

static int traverse(Node *node)
{
  // static int node_count = 0;
  static int count = 0;
  for (size_t i = 0; i < node->keys_size; i++)
  {
    printf("%i\n", node->keys[i]);
    count++;
  }
  // node_count++;
  puts("-------------------------");

  for (size_t i = 0; i < node->children_size; i++)
  {
    traverse(node->children[i]);
  }
  puts("------------ROOOOTTTT-------------");
  return count;
}

int main()
{
  Tree *btree = (Tree *)malloc(sizeof(Tree));
  btree->root = init_node(btree);
  printf("rootttt is: %p\n", btree->root);

  srand((unsigned)time(NULL));

  unsigned search_val = 88568084;
  const unsigned search_index = (unsigned)rand() % 30;

  // for (int i = 0; i < 30; i++)
  // {
  //   const unsigned num = (unsigned)rand() % 100000000;
  //   if (i == search_index)
  //   {
  //     search_val = num;
  //   }
  //   // printf("%i, ", num);
  //   node_insert(btree->root, num, 0);
  // }

  int sample[30] = {85129233, 44285551, 91196772, 21894069, 58800179, 84805012, 54347833, 73555921, 59664066, 80165136, 96781459, 60860634, 7650198, 95647012, 14541306, 68551340, 4419565, 78467934, 36678503, 37565498, 99621957, 64867130, 81232257, 35221242, 38998745, 86188568, 67284298, 84081141, 88568084, 79300238};
  for (int i = 0; i < 30; i++)
  {
    node_insert(btree->root, sample[i], 0);
  }

  traverse(btree->root);

  // delete (btree, search_val);
  delete (btree, 73555921);
  // delete (btree, 38998745);
  // delete (btree, 58800179);

  puts("------------------------After deletion-------------------\n");

  traverse(btree->root);

  SearchResult *result = search(btree, search_val);
  if (result)
    printf("found key %i", *result->key);
  else
    printf("couldn't find key %u\n", search_val);
}
