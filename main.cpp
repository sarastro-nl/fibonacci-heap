#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" {

#define NROFDIGITS(x) ({size_t i = 1; int y = (x); while ((y) > 9) { (y) /= 10; i++; }; i; })
#define STRLENGTH(s) ({size_t i = 0; const char *y = (s); while (*y) {i += (*y++ & 0xC0) != 0x80;}; i; })
#define MININF -INT32_MAX

struct node_t {
    bool mark;
    int value;
    int degree;
    node_t* left;
    node_t* right;
    node_t* child;
    node_t* parent;
};

static struct {
    node_t* min;
    node_t* root;
    int n;
} heap;

void print_heap();

void add_to_root(node_t *node) {
    if (heap.root == NULL) {
        heap.root = node;
        node->left = node;
        node->right = node;
    } else {
        node->left = heap.root->left;
        node->right = heap.root;
        heap.root->left->right = node;
        heap.root->left = node;
    }
}

node_t *insert(int value) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->value = value;
    add_to_root(node);
    heap.n++;
    if (heap.min == NULL || value < heap.min->value) {
        heap.min = node;
    }
    return node;
}

int minimum() {
    return heap.min->value;
}

void add_child(node_t *node, node_t *child) {
    child->left->right = child->right;
    child->right->left = child->left;
    child->parent = node;
    if (heap.root == child) {
        heap.root = child->right;
    }
    if (node_t* first_child = node->child) {
        child->left = first_child->left;
        child->right = first_child;
        first_child->left->right = child;
        first_child->left = child;
    } else {
        node->child = child;
        child->left = child;
        child->right = child;
    }
    node->degree++;
}

node_t **get_siblings(node_t *node) {
    node_t **siblings = (node_t **)calloc((size_t)heap.n, sizeof(node_t *));
    if (node_t *start = node) {
        int i = 0;
        do {
            siblings[i++] = node;
            node = node->right;
        } while (node != start);
    }
    return siblings;
}

void consolidate() {
    printf("consolidating\n");
    node_t **array = (node_t **)calloc((size_t)heap.n, sizeof(node_t *));
    node_t **siblings = get_siblings(heap.root);
    int i = 0;
    while (node_t *node = siblings[i++]) {
        int d = node->degree;
        while (node_t *other = array[d]) {
            if (other->value < node->value) {
                node_t *temp = other;
                other = node;
                node = temp;
            }
            add_child(node, other);
            print_heap();
            array[d] = NULL;
            d++;
        }
        array[d] = node;
    }
    free(array);
    free(siblings);
}

node_t *remove_minimum() {
    node_t *min = heap.min;
    if (min) {
        printf("removing minimum: ");min->value == MININF ? printf("-inf\n") : printf("%d\n", min->value);
        node_t **siblings = get_siblings(min->child);
        int i = 0;
        while (node_t *child = siblings[i++]) {
            add_to_root(child);
            child->parent = NULL;
        }
        free(siblings);
        min->left->right = min->right;
        min->right->left = min->left;
        if (min == min->right) {
            heap.root = NULL;
            heap.min = NULL;
        } else {
            if (heap.root == min) {
                heap.root = min->right;
            }
            print_heap();
            consolidate();
            heap.min = heap.root;
            node_t *node = heap.root->right;
            while (node != heap.root) {
                if (node->value < heap.min->value) {
                    heap.min = node;
                }
                node = node->right;
            }
        }
        heap.n--;
    }
    return min;
}

void move_to_root(node_t *node, node_t *parent) {
    node->left->right = node->right;
    node->right->left = node->left;
    if (parent->child == node) {
        if (node == node->right) {
            parent->child = NULL;
        } else {
            parent->child = node->right;
        }
    }
    add_to_root(node);
    print_heap();
    parent->degree--;
    node->parent = NULL;
    node->mark = false;
}

void cascade(node_t *node) {
    if (node_t *parent = node->parent) {
        if (!node->mark) {
            node->mark = true;
        } else {
            move_to_root(node, parent);
            cascade(parent);
        }
    }
}

void change_value(node_t *node, int value) {
    printf("changing value: %d to ", node->value);
    value == MININF ? printf("-inf\n") : printf("%d\n", value);
    int old_value = node->value;
    node->value = value;
    print_heap();
    if (value < old_value) {
        node_t *parent = node->parent;
        if (parent && value < parent->value) {
            move_to_root(node, parent);
            cascade(parent);
        }
        if (value < heap.min->value) {
            heap.min = node;
        }
    }
}

node_t *remove_node(node_t *node) {
    change_value(node, MININF);
    return remove_minimum();
}

void print_nodes(size_t *indent, size_t *maxNrOfChars, char **a, int row, node_t *node) {
    char buf[BUFSIZ];
    char *s = a[row];
    if (row > 0) {
        size_t diff = *indent - STRLENGTH(s);
        snprintf(buf, diff + 1, "%*s", (int)diff, "");
        strcat(s, buf);
        strcat(s, "│");
        s = a[++row];
        diff = *indent - STRLENGTH(s);
        snprintf(buf, diff + 1, "%*s", (int)diff, "");
        strcat(s, buf);
    }
    node_t *start = node;
    do {
        char buf[BUFSIZ];
        size_t nrOfChars;
        if (node->value == MININF) {
            nrOfChars = 6;
            strcpy(buf, "(-inf)");
        } else {
            nrOfChars = NROFDIGITS(node->value);
            snprintf(buf, nrOfChars + 1, "%d", node->value);
        }
        strcat(s, buf);
        if (nrOfChars + 1 > *maxNrOfChars) {
            *maxNrOfChars = nrOfChars + 1;
        }
        if (node_t *child = node->child) {
            print_nodes(indent, maxNrOfChars, a, row + 1, child);
        } else {
            *indent += *maxNrOfChars;
            *maxNrOfChars = 0;
        }
        if (start != node->right) {
            size_t diff = *indent - STRLENGTH(s);
            memset(buf, '-', diff);
            buf[diff] = 0;
            strcat(s, buf);
        }
        node = node->right;
    } while (start != node);
}

char s[BUFSIZ * BUFSIZ];
void print_heap() {
    char *a[BUFSIZ];
    for (int i = 0; i < BUFSIZ; i++) {
        a[i] = s + sizeof(char) * (size_t)i * BUFSIZ;
        a[i][0] = 0;
    }
    if (node_t *node = heap.root) {
        size_t indent = 0;
        size_t maxNrOfChars = 0;
        print_nodes(&indent, &maxNrOfChars, a, 0, node);
    }
    int i = 0;
    while (a[i][0] != 0) {
        printf("%s\n", a[i++]);
    }
    printf("======================\n");
}

int main(int argc, const char * argv[]) {
    node_t *n1 = insert(7);
    node_t *n2 = insert(18);
    node_t *n3 = insert(38);
    node_t *n4 = insert(24);
    node_t *n5 = insert(17);
    node_t *n6 = insert(23);
    node_t *n7 = insert(21);
    node_t *n8 = insert(39);
    node_t *n9 = insert(41);
    node_t *n10 = insert(26);
    node_t *n11 = insert(46);
    node_t *n12 = insert(30);
    node_t *n13 = insert(35);
    node_t *n14 = insert(52);
    add_child(n1, n4);
    add_child(n1, n5);
    add_child(n1, n6);
    add_child(n4, n10);
    add_child(n4, n11);
    add_child(n5, n12);
    add_child(n10, n13);
    add_child(n2, n7);
    add_child(n2, n8);
    add_child(n7, n14);
    add_child(n3, n9);
    n10->mark = true;
    n2->mark = true;
    n8->mark = true;
    print_heap();
    change_value(n11, 15);
    print_heap();
    change_value(n13, 5);
    print_heap();
    remove_node(n14);
    print_heap();
    while (node_t *node = remove_minimum()) {
        free(node);
    }
    return 0;
}

}
