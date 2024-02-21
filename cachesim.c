#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct CacheBlock{
    int tag;
    unsigned int* data; 
    // int index;
     
    int valid;
    int LRU; 

    
};

struct CacheBlock **cachefile; 

// unsigned int* memory;
unsigned int memory[16777216];

void initData(int sets, int assoc, int blockSize){

    cachefile = (struct CacheBlock **)calloc(sets, sizeof(struct MyStruct *));
     for (int i = 0; i < sets; i++) {
        cachefile[i] = (struct CacheBlock *)calloc(assoc, sizeof(struct CacheBlock));
     }
     for (int i = 0; i < sets; ++i) {
        for (int j = 0; j < assoc; ++j) {
            cachefile[i][j].tag = 0;
            cachefile[i][j].data = (unsigned int*)calloc(blockSize, sizeof(unsigned int));
            cachefile[i][j].valid = 0;
            cachefile[i][j].LRU = 0;
            // Initialize other members as needed
        }
    }

    
        // memory = (unsigned int*) malloc(1677216*sizeof(unsigned int*));
        
    
}

void freeData(int sets, int assoc, int blockSize){

    cachefile = (struct CacheBlock **)calloc(sets, sizeof(struct MyStruct *));
     for (int i = 0; i < sets; i++) {
        cachefile[i] = (struct CacheBlock *)calloc(assoc, sizeof(struct CacheBlock));
     }
     for (int i = 0; i < sets; ++i) {
        for (int j = 0; j < assoc; ++j) {
            free(cachefile[i][j].data);
        }
    }

    
        // memory = (unsigned int*) malloc(1677216*sizeof(unsigned int*));
        
    
}

unsigned int* fetchBlock(int assoc, int tag, int clk, int indexVal){
    for (int j = 0; j < assoc; ++j) {
        if(cachefile[indexVal][j].tag == tag && cachefile[indexVal][j].valid == 1){
            cachefile[indexVal][j].LRU = clk;
            return cachefile[indexVal][j].data;
        }
    }
    return NULL;
}

void removeLRU(int index, int assoc){
     int minLRU = cachefile[index][0].LRU; 
                    int evictWay = 0;
                    
        for(int j = 0; j < assoc; ++j) {
            if(cachefile[index][j].LRU<minLRU){
                 minLRU = cachefile[index][j].LRU;
                 evictWay = j;
            }
        }
    cachefile[index][evictWay].valid=0;
}

void cashingBlock(int index, int tag, int blkSize, unsigned int* fullBlk, int clk, int assoc){
        int insert = 0;
        int wayFoundInsert = 0;

        for(int j = 0; j < assoc; ++j) {
        if(cachefile[index][j].valid==0){
            insert = 1;
            wayFoundInsert = j;
            break;
            }
        }
        if(insert==0){
            removeLRU(index, assoc);
            for(int j=0; j<assoc; j++){
                if(cachefile[index][j].valid == 0){
                    wayFoundInsert = j;
                    insert = 1;
                }
            }
        }

        if(insert==1){
            memcpy(cachefile[index][wayFoundInsert].data, fullBlk, sizeof(unsigned int) * blkSize);
            cachefile[index][wayFoundInsert].LRU = clk;
            cachefile[index][wayFoundInsert].tag = tag;
            cachefile[index][wayFoundInsert].valid = 1;

        }
}



int log2(int n) {
int r=0;
while (n>>=1) r++;
return r;
}

int main (int argc, char *argv[]){ 
    FILE *file;

    char filename[32];
    int clk = 0;

    unsigned int cachesizeKB; 
    long long cachesize;
    int assoc;
    int blockSize;  

    int numFrames; 
    int sets;


    sscanf(argv[1], "%s", filename);
    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    cachesizeKB = atoi(argv[2]);  
    cachesize = (long long) cachesizeKB << 10;

    assoc = atoi(argv[3]);
    blockSize = atoi(argv[4]);

    numFrames = (int) cachesize / blockSize;
    sets = (int) numFrames/assoc;

    int offsetBits = (int)log2(blockSize);
    int indexBits = (int)log2(sets);

    initData(sets, assoc, blockSize);


    int result;  
    unsigned int hexAd;
    int bytes;
    long long dataIn;
    char line[64];


    while (fscanf(file, "%s", line) != EOF) {
       
        clk++;

        if(strcmp(line, "store")==0){

            fscanf(file, "%x", &hexAd);
            fscanf(file, "%d", &bytes);
            fscanf(file, "%llx", &dataIn);

            unsigned int blockStartAdr = hexAd & ~((1<<offsetBits)-1);
            unsigned int blk_offset = hexAd & ((1<<offsetBits)-1);
            unsigned int index =  (blockStartAdr >> offsetBits) & ((1<<indexBits)-1);
            unsigned int tagIn = blockStartAdr >> (offsetBits+indexBits);

     
            unsigned int putDataMem[bytes];
            int hitOrMiss = 0;

      
            for (int j = 0; j<bytes; j++) {
             // Extract each byte using bitwise operations  
                int parsed_to_store = (dataIn >> ((bytes - 1 - j) * 8)) & 0xFF;
                // printf("%d %d\n", hexAd + j ,parsed_to_store);
                putDataMem[j] = parsed_to_store;
                memory[hexAd + j] = parsed_to_store;
                 
            }

            unsigned int* cacheBlk = fetchBlock(assoc, tagIn, clk, index);

            if(cacheBlk!=NULL){

                memcpy(cacheBlk, putDataMem, sizeof(unsigned int) * bytes);
                hitOrMiss = 1;


            } 

            if(hitOrMiss == 1){
                printf("store 0x%x hit\n", hexAd);

            } else{
                printf("store 0x%x miss\n", hexAd);
            }

        } else if(strcmp(line, "load")==0){
            fscanf(file, "%x", &hexAd);
            fscanf(file, "%d", &bytes);

            unsigned int blockStartAdr = hexAd & ~((1<<offsetBits)-1);
            unsigned int blk_offset = hexAd & ((1<<offsetBits)-1);
            unsigned int index =  (blockStartAdr >> offsetBits) & ((1<<indexBits)-1);
            unsigned int tagIn = blockStartAdr >> (offsetBits+indexBits);

            unsigned int* cacheBlk = fetchBlock(assoc, tagIn, clk, index);
            unsigned int getCacheData[bytes];
            int hitOrNotLoad = 0;


    
             if(cacheBlk != NULL){
                    // memcpy(getCacheData, cacheBlk, sizeof(unsigned int) *  bytes);
                    hitOrNotLoad = 1;

                } else {
                
               
                    unsigned int* dataBlock = (unsigned int*)calloc(blockSize, sizeof(unsigned int));
                    memcpy(dataBlock, &memory[hexAd], sizeof(unsigned int) * blockSize);
                    // memcpy(getCacheData, &memory[hexAd], sizeof(unsigned int) * bytes);
                    cashingBlock(index, tagIn, blockSize, dataBlock, clk, assoc);

               
                    // memcpy(getCacheData, dataBlock+blk_offset, bytes);
                    free(dataBlock);

                }
            if(hitOrNotLoad==1){
                printf("load 0x%x hit ", hexAd);
                for(int j = 0; j<bytes;j++ ){
                    printf("%02x", memory[hexAd+j]);
                }
                printf("\n");
            } else {
                printf("load 0x%x miss ", hexAd);
                for(int j = 0; j<bytes;j++ ){
                     printf("%02x", memory[hexAd+j]);
                }
                printf("\n");
            }

            
        }
      
    }
    freeData(sets, assoc, blockSize);
    fclose(file);
    return(0);
}

    
// Load
// 1. go to the cache at the given address
// 2. if there is nothing (miss) but bring it from memory and save it in this location. Set valid tag to 1 You are bringing a whole block
// 3. if the valid bit is 1 and the tag is correct, print the thing with a hit

// steps for a miss load: 
// fetch from memory the substring 

  








