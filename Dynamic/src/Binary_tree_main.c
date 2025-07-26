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
    struct tree_node *parent_add;
    int data;
    LL_node *Child_LL_head_ptr;
    
}Tree_node;


/*---------------------------linked list part---------------------------- */

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
    return temp;
}
