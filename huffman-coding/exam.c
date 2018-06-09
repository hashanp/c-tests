#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exam.h"

/*
 * Private function prototypes.
 */

static void _print_huffman_tree(const huffman_tree_t *, int);
static void _print_huffman_tree_codes(const huffman_tree_t *, char *, char *);

/*
 * Prints the given Huffman tree.
 */
void print_huffman_tree(const huffman_tree_t *t) {
  printf("Huffman tree:\n");
  _print_huffman_tree(t, 0);
}

/*
 * Private helper function for print_huffman_tree.
 */
static void _print_huffman_tree(const huffman_tree_t *t, int level) {
  int i;
  for (i = 0; i <= level; i++) {
    printf("  ");
  }

  if (t->left == NULL && t->right == NULL) {
    printf("Leaf: '%c' with count %d\n", t->letter, t->count);
  } else {
    printf("Node: accumulated count %d\n", t->count);

    if (t->left != NULL) {
      _print_huffman_tree(t->left, level + 1);
    }

    if (t->right != NULL) {
      _print_huffman_tree(t->right, level + 1);
    }
  }
}

/*
 * Prints the codes contained in the given Huffman tree.
 */
void print_huffman_tree_codes(const huffman_tree_t *t) {
  printf("Huffman tree codes:\n");

  char *code = calloc(MAX_CODE_LENGTH, sizeof(char)), *code_position = code;
  if (code == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  _print_huffman_tree_codes(t, code, code_position);
  free(code);
}

/*
 * Private helper function for print_huffman_tree_codes.
 */
static void _print_huffman_tree_codes(const huffman_tree_t *t,
                                      char *code, char *code_position) {

  if (t->left == NULL && t->right == NULL) {
    printf("'%c' has code \"%s\"\n", t->letter, code);
  } else if (t->left != NULL) {
    *code_position++ = 'L';
    *code_position = '\0';

    _print_huffman_tree_codes(t->left, code, code_position--);
  }

  if (t->right != NULL) {
    *code_position++ = 'R';
    *code_position = '\0';

    _print_huffman_tree_codes(t->right, code, code_position--);
  }
}

/*
 * Prints a list of Huffman trees.
 */
void print_huffman_tree_list(const huffman_tree_list_t *l) {
  printf("Huffman tree list:\n");

  for (; l != NULL; l = l->next) {
    print_huffman_tree(l->tree);
  }
}

/*
 * Frees a Huffman tree.
 */
void huffman_tree_free(huffman_tree_t *t) {
  if (t != NULL) {
    huffman_tree_free(t->left);
    huffman_tree_free(t->right);
    free(t);
  }
}

/*
 * Frees a list of Huffman trees.
 */
void huffman_tree_list_free(huffman_tree_list_t *l) {
  if (l != NULL) {
    huffman_tree_list_free(l->next);
    huffman_tree_free(l->tree);
    free(l);
  }
}

/*
 * Returns 1 if the string s contains the character c and 0 if it does not.
 */
int contains(char *s, char c) {
  assert(s != NULL);
  while (*s != '\0') {
    if (*s == c) {
      return 1;
    }
    s++;
  }
  return 0;
}

/*
 * Returns the number of occurrences of c in s.
 */
int frequency(char *s, char c) {
  assert(s != NULL);
  int count = 0;
  while (*s != '\0') {
    if (*s == c) {
      count++;
    }
    s++;
  }
  return count;
}

/*
 * Takes a string s and returns a new heap-allocated string containing only the
 * unique characters of s.
 *
 * Pre: all strings will have fewer than or equal to MAX_STRING_LENGTH - 1
 *      characters.
 */
char *nub(char *s) {
  assert(s != NULL);
  int count = 0;
  char *t = malloc(MAX_STRING_LENGTH);
  while (*s != '\0') {
    int flag = 1;
    for (int i = 0; i < count; i++) {
      if (t[i] == *s) {
        flag = 0; 
        break;
      }
    }
    if (flag) {
      t[count++] = *s;
    }
    s++;
  }
  t[count] = '\0';
  return t;
}

/*
 * Adds the Huffman tree t to the list l, returning a pointer to the new list.
 *
 * Pre:   The list l is sorted according to the frequency counts of the trees
 *        it contains.
 *
 * Post:  The list l is sorted according to the frequency counts of the trees
 *        it contains.
 */
huffman_tree_list_t *huffman_tree_list_add(huffman_tree_list_t *l,
                                            huffman_tree_t *t) {
  assert(t != NULL);
  huffman_tree_list_t *nova = malloc(sizeof(huffman_tree_t));
  assert(nova != NULL);
  nova->tree = t;
  if (l == NULL || l->tree->count > t->count) {
    nova->next = l;
    return nova;
  }
  huffman_tree_list_t *curr = l;
  while (curr->next != NULL && curr ->next->tree->count < t->count) {
    curr = curr->next;
  }
  nova->next = curr->next;
  curr->next = nova;
  return l;
}

/*
 * Takes a string s and a lookup table and builds a list of Huffman trees
 * containing leaf nodes for the characters contained in the lookup table. The
 * leaf nodes' frequency counts are derived from the string s.
 *
 * Pre:   t is a duplicate-free version of s.
 *
 * Post:  The resulting list is sorted according to the frequency counts of the
 *        trees it contains.
 */
huffman_tree_list_t *huffman_tree_list_build(char *s, char *t) {
  assert(s != NULL);
  assert(t != NULL);
  huffman_tree_list_t *l = NULL;
  for (int i = 0; i < strlen(t); i++) {
    huffman_tree_t *v = malloc(sizeof(huffman_tree_t));
    assert(v != NULL);
    char p = t[i];
    v->count = frequency(s, p);
    v->letter = p;
    v->left = NULL;
    v->right = NULL;
    l = huffman_tree_list_add(l, v);
  }
  return l;
}

/*
 * Reduces a sorted list of Huffman trees to a single element.
 *
 * Pre:   The list l is non-empty and sorted according to the frequency counts
 *        of the trees it contains.
 *
 * Post:  The resuling list contains a single, correctly-formed Huffman tree.
 */
huffman_tree_list_t *huffman_tree_list_reduce(huffman_tree_list_t *l) {
  assert(l != NULL);
  while (l->next != NULL) {
    huffman_tree_t *t1 = l->tree;
    huffman_tree_t *t2 = l->next->tree;
    huffman_tree_t *nova = malloc(sizeof(huffman_tree_t));
    assert(nova != NULL);
    nova->count = t1->count + t2->count;
    nova->letter = '\0';
    nova->left = t1;
    nova->right = t2;
    huffman_tree_list_t *n = l->next->next;
    free(l->next);
    free(l);
    l = n;
    l = huffman_tree_list_add(n, nova);
  }
  return l;
}

int depth(huffman_tree_t *t) {
  assert(t != NULL);
  if (t->left == NULL) {
    return 1;
  }
  return depth(t->left) + depth(t->right);
}

#define MAX(a, b) ((a) < (b) ? (b) : (a))

int height(huffman_tree_t *t) {
  assert(t != NULL);
  if (t->left == NULL) {
    return 0;
  }
  return 1 + MAX(height(t->left), height(t->right));
}

int complete(char** map, huffman_tree_t *t, int i, int j) {
  assert(map != NULL);
  assert(t != NULL);
  if (t->left == NULL) {
    map[i][j] = '\0';
    map[i][0] = t->letter;
    return 1;
  } else {
    int n1 = complete(map, t->left, i, j + 1);
    for (int k = i; k < i + n1; k++) {
      map[k][j] = 'L';
    }
    int n2 = complete(map, t->right, i + n1, j + 1);
    for (int k = i + n1; k < i + n1 + n2; k++) {
      map[k][j] = 'R';
    }
    return n1 + n2;
  }
}

/*
 * Accepts a Huffman tree t and a string s and returns a new heap-allocated
 * string containing the encoding of s as per the tree t.
 *
 * Pre: s only contains characters present in the tree t.
 */
char *huffman_tree_encode(huffman_tree_t *t, char *s) {
  assert(t != NULL);
  assert(s != NULL);
  int d = depth(t);
  int h = height(t);
  char **map = malloc(d * sizeof(char*));
  assert(map != NULL);
  for (int i = 0; i < d; i++) {
    map[i] = malloc(h + 2);
  }
  complete(map, t, 0, 1);
  char *v = calloc(MAX_CODE_LENGTH, 1);
  assert(v != NULL);
  int len = strlen(s);
  for (int i = 0; i < len; i++) {
    char p = s[i];
    for (int j = 0; j < d; j++) {
      if (map[j][0] == p) {
        strcat(v, &map[j][1]);
        break;
      }
    }
  }
  for (int i = 0; i < d; i++) {
    free(map[i]);
  }
  free(map);
  return v;
}

/*
 * Accepts a Huffman tree t and an encoded string and returns a new
 * heap-allocated string contained the decoding of the code as per the tree t.
 *
 * Pre: the code given is decodable using the supplied tree t.
 */
char *huffman_tree_decode(huffman_tree_t *t, char *code) {
  assert(t != NULL);
  assert(code != NULL);
  huffman_tree_t *curr = t;
  int len = strlen(code);
  char *str = malloc(MAX_STRING_LENGTH);
  assert(str != NULL);
  int j = 0;
  for (int i = 0; i < len; i++) {
    if (code[i] == 'L') {
      curr = curr->left;
    } else {
      curr = curr->right;
    }
    if (curr->left == NULL) {
      str[j++] = curr->letter;
      curr = t;
    }
  }
  str[j] = '\0';
  return str;
}
