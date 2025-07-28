#include <stdio.h>
#include<stdlib.h>

//linked list  node 
typedef struct linked_list_node {

    struct linked_list_node * prev; 
    Tree_node *tree_node_add;
    struct linked_list_node * next;      
    
         
} LL_node;


//treee node structre  
typedef struct tree_node
{
    struct tree_node *parent_node_add;
    int data;
    LL_node *Child_LL_head_ref;
    
}Tree_node;


/*---------------------------Address linked list part---------------------------- */

//creating  linkedlist node 

LL_node *create_node(Tree_node*tree_node_add){
    LL_node *new_node=(LL_node*)malloc(sizeof(LL_node));
    
    if(!new_node){
        printf("Memory allocatioin failed \n");
        exit(1);
    
    }
    new_node->prev=NULL;
    new_node->tree_node_add=tree_node_add;
    new_node->next=NULL;
}

int LL_len(LL_node**head_ref){
    LL_node *temp=*head_ref;
    int count =0;
    while(temp!=NULL){
        count++;
        temp=temp->next;
    }
    return count;
}



void LL_add_beg(LL_node** head_ref,Tree_node* tree_node_add){

    LL_node *new_node=create_node(tree_node_add);

    if(*head_ref==NULL){
        *head_ref=new_node;
    }

    else {
        new_node->next=*head_ref;
        *head_ref=new_node;
    }
}

void LL_add_at(LL_node **head_ref,Tree_node* tree_node_add,int pos){

    //empty conditionn 
    if(*head_ref==NULL && pos==0){
        LL_add_beg(head_ref,tree_node_add);
        return;
    }
    
    //position validity
    if(pos<0||pos>LL_len(head_ref)){
        printf("Incorrect position!");
        return;
    }
    //insertion between two nodes
    LL_node *temp=*head_ref;
    for(int i=0;i<pos-1 && temp!=NULL;i++){ //traverse untill pos-1
        temp=temp->next;
    }
    LL_node*new_node=create_node(tree_node_add);//node creations 
    //node linking part 
    new_node->prev=temp;
    new_node->next=temp->next;

    //safegurd incase of null pointer 
    if(temp->next !=NULL){
        temp->next->prev=new_node;
    }
    temp->next=new_node;
}



void LL_add_end(LL_node**head_ref,Tree_node *tree_node_add){
    LL_node *temp=*head_ref;
    LL_node *new_node=create_node(tree_node_add);

    if(*head_ref==NULL){
        *head_ref=new_node;
        return;
    }
    else{
        //traverse at the last node
        while (temp->next!=NULL)
        {
            temp=temp->next;
        }
        temp->next=new_node;
        new_node->prev=temp;
         
    }
}

LL_node *LL_del_beg(LL_node**head_ref){
    if(*head_ref==NULL){
        printf("Empty list !!");
        return NULL;
    }
    LL_node *temp=*head_ref;
    *head_ref=temp->next;
    //safegurd
    if(*head_ref!=NULL){
        (*head_ref)->prev=NULL;

    }
    temp->prev=NULL;
    temp->next=NULL;
    return temp; //return the deleted linkedlist node  
}



LL_node *LL_del_at(LL_node** haead_ref,int pos ){
    //condittion for emptylist
    if(*haead_ref==NULL){
        printf("list is already empty!");
        return NULL;
    }
    //contains single node 
    if(pos==0){
        return LL_del_beg(haead_ref);
    }
    //invalid position
    if(pos<0||pos>LL_len(haead_ref)){
        printf("Invalid position!");
        return NULL;
    }
    //Deleting node 
    LL_node *temp=*haead_ref;
    for(int i=0;i<pos && temp!=NULL;i++){ //traversing to pos-1
        temp=temp->next;
    }
    //safegurd
    if(temp==NULL){return NULL;}

    //linking and updating 
    if(temp->prev!=NULL){  
        temp->prev->next=temp->next;
    }
    if(temp->next!=NULL){
        temp->next->prev=temp->prev;
    }
    temp->prev=NULL;
    temp->next=NULL;
    return temp; //return the isolated  linkedlist node 
}

LL_node *LL_del_end(LL_node **head_ref){
    //empty list 
    if(*head_ref==NULL){
        printf("Empty list");
        return NULL;
    }
    LL_node *temp=*head_ref; //holding the Head pointer 
    //traversal 
    while (temp->next!=NULL)
    {   
        temp=temp->next;
        
    }
    //update links 
    if(temp->prev!=NULL){
        temp->prev->next=NULL; //temp is isolated 
    }
    else{
        *head_ref=NULL; //incase the list contains one node only 
    }

    temp->prev=NULL;
    temp->next=NULL;
    return temp;//return the isolated linkedlist node 

}

void LL_peek(LL_node **head_ref){
    if(*head_ref==NULL){
        printf("[Null]");
        return;
    }
    else{
        LL_node *temp=*head_ref;
        while (temp!=NULL)
        {
            printf("[%p]->", (void *)temp->tree_node_add);
            temp=temp->next;
        }
        printf("Null\n");
    }
}


// Address Queue part 
void enqueue(LL_node **head_ref,Tree_node *tree_node_add){
    if (*head_ref==NULL){
        LL_add_beg(head_ref,tree_node_add);
    }
    else{
        LL_add_end(head_ref,tree_node_add);
    }
}

LL_node* dequeue(LL_node**head_ref){
    if(*head_ref==NULL){
        printf("Empty Queue!");
    }
    else{
        LL_node* temp_LL_node=LL_del_beg(head_ref);
        return temp_LL_node;
    }
}



//Tree part :

Tree_node *create_tree_node(int data){
    
    Tree_node *new_tree_node=(Tree_node*)malloc(sizeof(Tree_node));
    if(!new_tree_node){
        printf("Memory allocation for tree node failed");
        exit(1);
    }
    new_tree_node->parent_node_add=NULL;
    new_tree_node->data=data;
    new_tree_node->Child_LL_head_ref=NULL;
    return new_tree_node;

}

// creation of tree 
void add_root(LL_node**LL_head_ref,LL_node**Q_head_ref){ 
    printf("Input the tree node value : ");
    int data=0;
    scanf("%d",&data);
    Tree_node *new_tree_node=create_tree_node(data);
    enqueue(Q_head_ref,create_tree_node); //root tree node add is pushed to Queue
    add_child(LL_head_ref,Q_head_ref,&new_tree_node);
    

}

void add_child(LL_node **LL_head_ref,LL_node **Q_head_ref,LL_node**parent_node_add){

    printf("Input the child node value : ");
    int data=0;
    scanf("%d",&data);
    //double while loop will be used to create treee 
}