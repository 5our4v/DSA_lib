#include <stdio.h>
#include <stdlib.h>


//linked list part 

typedef struct node 
{
    int data ; 
    struct node *right;
    struct node *left;

}Node;
 
Node *create_node(int data ){
    Node *new_node=(Node*)malloc(sizeof(Node));
    if(!new_node){
        printf("Memory allocation falied \n");
        exit(1);
    }
    new_node->data=data;
    new_node->left=NULL;
    new_node->right=NULL;
    return new_node;
}
 
void ll_add_beg(Node **head_ref,int value){      // add_beg(address of the head pointer ,data)
    Node *new_node=create_node(value);

    if(*head_ref==NULL){
        *head_ref=new_node;
    }
    else{
        new_node->right=*head_ref;
        *head_ref=new_node;
    }
}

int ll_len (Node **head_ref ){
    Node*temp=*head_ref;
    int count=0;
    while(temp!=NULL){
        count++;
        temp=temp->right;
    }
    return count;
}

void ll_add_at(Node **head_ref,int data ,int pos){
    //empty condition 
    if (*head_ref == NULL && pos == 0){
        ll_add_beg(head_ref, data);
        return;
    }

    //postion validity 
    if(pos<0 ||pos>ll_len(head_ref)){
        printf("position is out of bounds !!");
        return;
    }  
    //inserting node between two nodes
    Node *temp = *head_ref;

    //used for traversing the list till pos-1
    for (int i = 0; (i < pos - 1 && temp != NULL); i++)
    {
        temp = temp->right;
    }

    Node *new_node = create_node(data);
    //Node linking part
    new_node->left = temp;
    new_node->right = temp->right;

    //safegurd for Null pointers
    if (temp->right != NULL)
    {
        temp->right->left = new_node;
    }
    temp->right = new_node;

}

void ll_add_end(Node **head_ref,int data ){

    Node *temp=*head_ref; 
    Node *new_node=create_node(data);

    if(*head_ref==NULL){
        *head_ref=new_node;
        return;
    }
    else{
        //traverse at the end of the list 
        while(temp->right!=NULL){
            temp=temp->right;
        }
        temp->right=new_node;
        new_node->left=temp;
    }

}

void ll_del_beg(Node **head_ref){
    if(*head_ref==NULL){
        printf("List is empty!!");
        return;
    }
    Node *temp=*head_ref;
    *head_ref=temp->right; //updating the head pointer 
    //gurd
    if(*head_ref!=NULL){
        (*head_ref)->left=NULL;
    }
    free(temp); //delete the node 
}

void ll_del_at(Node**head_ref,int pos){
    //condition for emptylist 
    if(*head_ref==NULL){
        printf("List is already empty !");
        return;
    }
    //contains single node
    if(pos==0){
        ll_del_beg(head_ref);
        return;
    }
    //out of bound 
    if(pos<0||pos>=ll_len(head_ref)){
        printf("Position is out of bounds !!");
        return;
    }
    //Deleting node 
    Node *temp=*head_ref;
    for(int i=0;i<pos &&temp!=NULL;i++){ //traversing to the pos-1
        temp=temp->right;
    }

    //position is the last node 
    if(temp==NULL){
        return;
    }
    //linking and updating 
    if(temp->left!=NULL){temp->left->right=temp->right;}
    if(temp->right!=NULL){temp->right->left=temp->left;}
    free(temp);
}

void ll_del_end(Node **head_ref){
    //empty list 
    if(*head_ref==NULL){
        printf("The list is empty !");
        return;
    }

    Node *temp=*head_ref; //holding head pointer 
    //traversal 
    while (temp->right!=NULL){temp=temp->right;}

    //update links 
    if(temp->left!=NULL){
        temp->left->right=NULL;
        temp->left=NULL;
    }
    else{
        *head_ref=NULL; //list contains one node only
    }
    free(temp);
}

//prints the list content head to tail
void ll_peek (Node**head_ref){
   
   
   if(*head_ref==NULL){
    printf("[Null]");
    return;
   }
   else{
     Node *temp=*head_ref;
     int length=ll_len(head_ref);
    printf("length=%d \n",length);
     while (temp!=NULL)
     {
        printf("[%d]->",temp->data);
        temp=temp->right;
     }
     printf("Null\n");
   }
  
}

void ll_free_list(Node **head_ref)
{
    Node *temp = *head_ref;
    Node *right;

    while (temp != NULL)
    {
        right = temp->right; // Save right node
        free(temp);        // Free current node
        temp = right;       // Move to right
    }

    *head_ref = NULL; // Reset head pointer
}

//--------------------------------------------------------------------------

//Queue part 
void enqueue(Node **head_ref,int data ){
    
    if(*head_ref==NULL){
        ll_add_beg(head_ref,data);
    }
    else {
        ll_add_end(head_ref,data);
    
    }
    

}

void dequeue(Node ** head_ref){
    if(*head_ref==NULL){
        printf("Queue is empty !");
    }
    else{
        ll_del_beg(head_ref);
    }
}

void display_queue(Node **head_ref){

    if(*head_ref==NULL){
        printf("|__|;");
    }
   else{
        Node *temp=*head_ref;
        printf("Start:\n");
        //traverse and prints  
        while(temp!=NULL){
            printf("|%d|<-",temp->data);
            temp=temp->right;
        }

   }

}

//---------------------------------------------------------------------
//stack part 
void push(Node **head_ref,int data){
    
    if(*head_ref==NULL){
        ll_add_beg(head_ref,data);
    }
    else{
        ll_add_end(head_ref,data);
    }
}

void pop(Node **head_ref){
    if(*head_ref==NULL){
        printf("Stack is empty!");
    }
    else{
        ll_del_end(head_ref);
    }
}

void display_stack(Node **head_ref){

    if(*head_ref==NULL){
        printf("|____|\n");
        return;
    }
    Node *temp=*head_ref;

    //forward traversing 
    while (temp->right!=NULL){
        temp=temp->right;
    } 
    
    // Traverse backwards and print
    while (temp != NULL) {
        printf("|%d|\n-----\n", temp->data);
        temp = temp->left;
    }
       
}


