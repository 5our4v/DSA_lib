#include <stdio.h>
#include <stdlib.h>



//Linked list node :
typedef struct  link_list_node{
    struct link_list_node *prev;
    Tree_node *tree_node_addr;
    struct link_list_node *next;

} LL_node;

//Tree node : 
typedef struct  tree_node{
    struct tree_node *parrent_tree_node_addr;
    int data;
    LL_node *child_ll_head_ref;
}Tree_node;

LL_node *global_addr_list_head=NULL; // Global linked list to keep track of all the tree node address its a linked list actually 


//Linked list functions : 

//create node 
LL_node *create_ll_node(Tree_node*tree_node_addr){
    LL_node *new_ll_node=(LL_node*)malloc(sizeof(LL_node)); //memory allocation 
    if(!new_ll_node){
        perror("Memory alocation for LL_node failed ");
        exit(1);
    }
    new_ll_node->prev=NULL;
    new_ll_node->tree_node_addr=tree_node_addr;
    new_ll_node->next=NULL;
    return new_ll_node;
}
//add node  at the begining of the list 
void LL_add_beg(LL_node **head_ref,Tree_node *tree_node_addr){
    LL_node *new_ll_node=create_ll_node(tree_node_addr);
    if(*head_ref==NULL){
        *head_ref=new_ll_node;
    }
    else{
        new_ll_node->next=*head_ref;
        (*head_ref)->prev=new_ll_node;
        *head_ref=new_ll_node;
    }
}

//add  node at the end of the list 
void LL_add_end(LL_node **head_ref,Tree_node *tree_node_addr){
    LL_node *new_ll_node=create_ll_node(tree_node_addr);

    if(*head_ref==NULL){
        *head_ref=new_ll_node;
    }
    else{
        LL_node *temp=*head_ref;
        while (temp->next!=NULL)//traversing to the last node of the linked list 
        {
            temp=temp->next;
        }
        temp->next=new_ll_node;
        new_ll_node->prev=temp;
        
    }
}


// Tree part :

//create tree node :

Tree_node *create_tree_node(int data){
    Tree_node *tn=(Tree_node*)(malloc(sizeof(Tree_node)));
    if(!tn){
        perror("Memory allocation failed for tree node !");
        exit(1);
    }
    tn->data=data;
    tn->parrent_tree_node_addr=NULL;
    tn->child_ll_head_ref=NULL;
    return tn;
}

// find any tree node addr  by data in the global addr list 
Tree_node *find_node(int data ){
    LL_node *temp=global_addr_list_head;
    while (temp)
    {
        if(temp->tree_node_addr->data==data){
            return temp->tree_node_addr;
        }
        temp=temp->next;

    }
    return NULL;
    
}


// Creates tree structure 

void add_node(int parrent_data,int child_data){

//incase tree is empty then creating the root 
if(global_addr_list_head==NULL){
    Tree_node *root_node=create_tree_node(child_data); //creating the root tree node 
    LL_add_beg(&global_addr_list_head,root_node);
    printf("Root created %d\n",child_data);
}

//if the root node already present 
else{
    //find parrent from the global tree node addr list by the tree node data
    Tree_node *p_addr=find_node(parrent_data); //finds the tree node addr using the data inside of that tree node 
    if(p_addr==NULL){
        printf("Parrent %d not found !");
        return;
    }
    // create child tree node 
    Tree_node *child_tree_node=create_tree_node(child_data);
    child_tree_node->parrent_tree_node_addr=p_addr;

    //add the newly created child tree node addr to the global tree node addr list
    LL_add_end(&global_addr_list_head,child_tree_node);

    //using p_addr->child_LL_head_ref  as a head pointer to a  new linked list which contains the address of child tree node unde the parrent node 
    LL_add_end(&p_addr->child_ll_head_ref,child_tree_node);
    }
}

//print the tree 
void print_tree(Tree_node *tn ,int level ,int last ){
    if (tn==NULL){
        return;
    }

    //draw branches
    if(level>0){
        for(int i=0;i<level-1;i++){
            printf("|   ");

            if(last){
                printf("|--");
            }
        }
    }

}