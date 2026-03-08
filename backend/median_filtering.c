#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    float dist;
    struct node* next;
} node;

static void push(node** head, float dist);
static void pop_first(node** head);
static int check_full(node* head, int max_node);
static void print_list(node* head);
static float find_median(node* head, int max_node);
static int compare(const void* a, const void* b) {
    float fa = *(const float*)a, fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

int main(){
    int max_node = 3;
    float dists[] = {1.0,0.54,1.0,1.0,0.7};
    node* head = NULL;
    // usually you would use a while loop to read elements because of the distance sensor
    // but here for demonstration i use a finite list and iterate over that
    for (int i = 0; i < 5; i++){
        push(&head, dists[i]);
        if (check_full(head, max_node)){
            pop_first(&head);
        }
        print_list(head);
        // here calculate median
        printf("Median: %f\n", find_median(head, max_node));
    }
}

static void push(node** head, float dist){
    //in this implementation for intellibike we push to the end so that in the sliding window the values are organized
    node* new_node = (node*)malloc(sizeof(node));
    new_node->dist = dist;
    new_node->next = NULL;
    if (*head == NULL){
        *head = new_node;
        return;
    }
    node* current = *head;
    while (current->next != NULL){
        current = current->next;
    }//cycle to last
    current->next = new_node;
}
static void pop_first(node** head) {
    if (*head == NULL) return;
    node* old_head = *head;
    *head = (*head)->next;
    free(old_head);
}
static int check_full(node* head, int max_node){
    node* current = head;
    int nnode = 0;
    while (current != NULL){
        nnode++;
        current = current->next;
    }
    if (nnode > max_node){
        return 1;
    }
    return 0;
}
static void print_list(node* head){
    node* current = head;
    while(current != NULL){
        printf("%f meters\n", current->dist);
        current = current->next;
    }
}

static float find_median(node* head, int max_node){
    if (head == NULL){
        return 0.0f; // should prob fix this but not sure how rendering works
    }
    float* distp = malloc(max_node* sizeof(float));
    node* current = head;
    int n = 0;
    // create list
    while(current != NULL){
        distp[n] = current->dist;
        n++;
        current = current->next;
    }
    qsort(distp,n,sizeof(float), compare);
    if (n % 2 != 0) {
        return distp[n / 2];
    } else {
        return (distp[n / 2 - 1] + distp[n / 2]) / 2.0f;
    }
    free(distp);
}
