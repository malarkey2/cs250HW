#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Building{
    char buildName[64];
    int sqfoot;
    float energy;
    struct Building *next;
};

void sortNodes(struct Building* firstNode, int loopcount){

    struct Building *resetToOne = firstNode;
    // struct Building *resetToOne = (struct Building*) calloc(1, sizeof(struct Building));
    // resetToOne = firstNode;

    struct Building *current;
    struct Building *nextNode;
    int tempSq;
    float tempEn;
    char *tempName;
    
     for (current = firstNode; current != NULL; current = current->next) {
        for (nextNode = current->next; nextNode != NULL; nextNode = nextNode->next) {

            float eff1 = current->energy/(float) current->sqfoot;
            float eff2 = nextNode->energy/(float) nextNode->sqfoot;

            //I will try pointer manipulation next! Not a fan of the wordiness
            if (eff1<eff2) {
                tempSq = current->sqfoot;
                tempEn = current->energy;
                tempName = current->buildName;

                current->sqfoot = nextNode->sqfoot;
                current->energy = nextNode->energy;
                strcpy(current->buildName, nextNode->buildName);

                nextNode->sqfoot = tempSq;
                nextNode->energy = tempEn;
                strcpy(nextNode->buildName, tempName);

            }
        }
    }

            // if(eff2 == eff1){
            //     if(strcmp(resetToOne->buildName, resetToOne->next->buildName)>0){

            //     tempSqfoot = resetToOne->sqfoot;
            //     tempEnergy = resetToOne->energy;

            //     resetToOne->sqfoot = (resetToOne->next)->sqfoot;
            //     resetToOne->energy = (resetToOne->next)->energy;

            //     (resetToOne->next)->sqfoot = tempSqfoot;
            //     (resetToOne->next)->energy = tempEnergy;


            //     }
            // }  

            

        }
    

    // free(resetToOne);






void printNodesNF(struct Building* nodeOne){

    if(nodeOne == NULL){
        printf("No nodes to print\n");
        // free(nodeOne);
        exit(0);
    }

    while (nodeOne != NULL){

        float eff = nodeOne->energy/(float) nodeOne->sqfoot;
        printf("%s has an efficiency of %f\n", nodeOne->buildName, eff);
        // free(nodeOne);
        
        nodeOne = nodeOne->next;
    }

}

void printNodes(struct Building* nodeOne){

    if(nodeOne == NULL){
        printf("No nodes to print\n");
        // free(nodeOne);
        exit(0);
    }

    while (nodeOne != NULL){

        float eff = nodeOne->energy/(float) nodeOne->sqfoot;
        printf("%s %f\n", nodeOne->buildName, eff);
        free(nodeOne);
        
        nodeOne = nodeOne->next;
    }

}

int main (int argc, char *argv[]){ 
    FILE *file;

    char filename[32];

    sscanf(argv[1], "%s", filename);
    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read the first three lines

    struct Building *current = (struct Building*) calloc(1,sizeof(struct Building));
    struct Building *head = (struct Building*) calloc(1,sizeof(struct Building));


    


    int looper = 1;

    char buildNamein[64];
    int sqfootin;
    float energyin;

    int loopctr = 1;

    while(looper==1){

        

        fscanf(file, "%s", buildNamein);   // Read a string (char array)

        if(strcmp(buildNamein, "DONE")==0){
            break;
        }
        fscanf(file, "%d", &sqfootin);   // Read an integer
        fscanf(file, "%f", &energyin); // Read a float

        struct Building *newNode = (struct Building*) malloc(sizeof(struct Building));

        

        newNode -> sqfoot = sqfootin;
        newNode -> energy = energyin;
        strcpy(newNode->buildName, buildNamein);
        newNode->next = NULL;

        if(loopctr==1){
            head = newNode;
        }

        current->next = newNode;
        current = current->next;

        
        loopctr++;
    }

    // printf("Pre-sorting order: \n");
    // printNodesNF(head);

    sortNodes(head, loopctr);

    // printf("POST-sorting order: \n");


    printNodes(head);



    return 0;


}

