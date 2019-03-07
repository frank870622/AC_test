#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define MAX 26 //假設只出現26個小寫英文字母
#define ROW 4
#define COLUMN 10
char pattern[ROW][COLUMN] = {"nihao", "hao", "hs", "hsr"};
char *s = (char *)"sdmfhsgnshejfgnihaofhsrnihao";

////Node
typedef struct Node
{
    int index; //儲存模式串的下標
    char x;
    struct Node *parent;
    struct Node *next[MAX];
    struct Node *fail;

} Node;
void nodeInitial(Node *);

//// QUEUE
typedef struct items
{
    Node *data;
    struct items *next; // points to next element
} ITEM;

typedef struct queue
{
    int size;
    ITEM *front, *end;
} QUEUE;

void initQueue(QUEUE *);
int queueIsEmpty(QUEUE *);
void pushQueue(QUEUE *, Node *);
Node *popQueue(QUEUE *);

////ACTree
typedef struct ACTree
{
    Node *root;
} ACTree;

void ACTreeInitial(ACTree *);
void ACTree_Add(const char *, int, ACTree *);
void ACTree_NodeToQueue(Node *, QUEUE *, ACTree *);
void ACTree_BuildFailPointer(ACTree *);
void ACTree_ACSearch(const char *, ACTree *);

int main()
{
    ACTree tree;
    ACTreeInitial(&tree);
    for (int i = 0; i < ROW; i++)
        ACTree_Add(pattern[i], i, &tree);
    ACTree_BuildFailPointer(&tree);
    printf("待配對字串為（依次5個一組的輸出）：\n");
    for (int i = 1; i <= strlen(s); i++)
    {
        printf("%c", s[i]);
        if (i % 5 == 0)
            printf("  ");
    }
    printf("\n\n");
    printf("配對結果如下：\n");
    printf("位置\t編號\t模式\n");
    ACTree_ACSearch(s, &tree);
    return 0;
}

void nodeInitial(Node *node)
{
    node->index = -1;
    node->fail = NULL;
    node->parent = NULL;
    for (int i = 0; i < MAX; i++)
        node->next[i] = NULL;
}

void initQueue(QUEUE *q)
{
    q->size = 0;
    q->front = q->end = NULL;
}
int queueIsEmpty(QUEUE *q)
{
    return q->size == 0;
}
int queueLength(QUEUE *q)
{
    return q->size;
}
void pushQueue(QUEUE *q, Node *y)
{
    ITEM *x = (ITEM *)malloc(sizeof(ITEM));
    x->data = y;
    x->next = NULL;
    if (q->front == NULL)
        q->front = x;
    else
        q->end->next = x;
    q->end = x;
    q->size++;
}
Node *popQueue(QUEUE *q)
{
    ITEM *x = q->front;
    Node *rel = x->data;
    q->front = x->next;
    if (q->front == NULL)
        q->end = NULL;
    q->size--;
    free(x);
    return rel;
}

void ACTreeInitial(ACTree *actree)
{
    actree->root = (Node *)malloc(sizeof(Node));
    nodeInitial(actree->root);
    actree->root->fail = actree->root;
}

void ACTree_Add(const char *ch, int index, ACTree *actree)
{
    int len = strlen(ch);
    if (len == 0)
        return;
    Node *p = actree->root;
    for (int i = 0; i < len; i++)
    {
        int k = ch[i] - 'a';
        if (p->next[k] == NULL)
        {
            p->next[k] = (Node *)malloc(sizeof(Node));
            nodeInitial(p->next[k]);
            p->next[k]->parent = p;
            p->next[k]->x = ch[i];
        }
        p = p->next[k];
    }
    p->index = index; //注意，在此保證輸入的模式串不重複,否則index會被覆蓋
} //第一步

void ACTree_NodeToQueue(Node *node, QUEUE *q, ACTree *actree)
{
    if (node != NULL)
    {
        for (int i = 0; i < MAX; i++)
        {
            if (node->next[i])
                pushQueue(q, node->next[i]);
        }
    }
} //
void ACTree_BuildFailPointer(ACTree *actree)
{
    QUEUE q;
    initQueue(&q);
    for (int i = 0; i < MAX; i++)
    {
        if (actree->root->next[i])
        {
            ACTree_NodeToQueue(actree->root->next[i], &q, actree);
            actree->root->next[i]->fail = actree->root;
        }
    }
    Node *parent, *p;
    char ch;
    while (!queueIsEmpty(&q))
    {
        p = q.front->data;
        ch = p->x;
        parent = p->parent;
        popQueue(&q);
        ACTree_NodeToQueue(p, &q, actree);
        while (1)
        {
            if (parent->fail->next[ch - 'a'] != NULL)
            {
                p->fail = parent->fail->next[ch - 'a'];
                break;
            }
            else
            {
                if (parent->fail == actree->root)
                {
                    p->fail = actree->root;
                    break;
                }
                else
                    parent = parent->fail->parent;
            }
        }
    }
} //第二步
void ACTree_ACSearch(const char *s, ACTree *actree)
{
    int len = strlen(s);
    if (len == 0)
        return;
    Node *p = actree->root;
    int i = 0;
    while (i < len)
    {
        int k = s[i] - 'a';
        if (p->next[k] != NULL)
        {
            p = p->next[k];
            if (p->index != -1)
                printf("%ld\t%d\t%s\n", i - strlen(pattern[p->index]) + 1, p->index, pattern[p->index]);
            i++;
        }
        else
        {
            if (p == actree->root)
                i++;
            else
            {
                p = p->fail;
                if (p->index != -1)
                    printf("%ld\t%d\t%s\n", i - strlen(pattern[p->index]) + 1, p->index, pattern[p->index]);
            }
        }
    }
    while (p != actree->root)
    {
        p = p->fail;
        if (p->index != -1)
            printf("%ld\t%d\t%s\n", i - strlen(pattern[p->index]) + 1, p->index, pattern[p->index]);
    }
} //第三步