#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct{
	int valid;
	int tag;
	
}Cache;

typedef struct{
	int valid[4];
	int tag[4];
	int age[4];
}Cache_t;

int get_tag_bits(int address, int s, int b, int blockSize){
	
	address = address >> (s + b);
	return (address & ((1<<((blockSize*8)-s-b))-1)); //return the tag
}

int get_index_bits(int address, int s, int b){
	
	return ((address >> b) & ((1<<s)-1)); //return set bits
}

int directMapping(int cacheSize, int blockSize, int splitFlag, int assocFlag){

	//int splitFlag;
	//int cacheSize;
	//int blockSize;
	//int assocFlag;
	int offsetSize;
	int indexSize;
	int tagSize;
	int blockNumber;
	int i;
	char id;
	unsigned int addr;
	int compulsoryMiss = 0;
	int iCompulsoryMiss = 0;
	int dCompulsoryMiss = 0;
	int miss = 0;
	int dmiss = 0;
	int imiss = 0;
	int hit = 0;
	float hitRate = 0;
	
	if (splitFlag)
	{
		cacheSize = cacheSize/2;
		blockNumber = cacheSize/blockSize;
	}
	else
		blockNumber = cacheSize/blockSize;

	Cache *cache = (Cache *) malloc(sizeof(Cache)*blockNumber);
	Cache dCache[blockNumber];
	Cache iCache[blockNumber];
	offsetSize = (int)(log(blockSize)/log(2));
	indexSize = (int)((float)log(blockNumber)/log(2));
	//printf("Number of lines=%d\n", blockNumber);
	//printf("Offset is=%d\n", offsetSize);
	//printf("Index Size = %d\n", indexSize);

	//Cache cache[blockNumber];
	if(!splitFlag){
		//indexSize = (int)((float)log(blockNumber)/log(2));
		
		for(i = 0; i < blockNumber; ++i){
			cache[i].tag = 0;
			cache[i].valid = 0;
			
		}
	}
	else{
		
		//indexSize = (int)((float)log(blockNumber)/log(2));
		
		for(i = 0; i < blockNumber; ++i){
			dCache[i].tag = 0;
			dCache[i].valid = 0;
			iCache[i].tag = 0;
			iCache[i].valid = 0;
		}
	}
	
FILE * tracefile; //pointer to file obj
	
tracefile = fopen ("trace.din","r");

if(tracefile == NULL){
	printf("Unable to open the trace file\n");
	return 0;
}
	//while(feof(tracefile)){
		char buffer[20];

		//fgets
		while(fgets(buffer,20, tracefile) != NULL){
		char *p;
		p = strtok(buffer," ");
		//printf("%s\n",p);	
		id = p[0];
		//printf("%c\n",id);
		char *pEnd;
		p = strtok(NULL, " ");
		if(p == NULL)
	    return 0;
		addr = (unsigned int)strtol(p, &pEnd , 16);
		//printf("Addr=%d\n", addr);
		int index_bits = get_index_bits(addr,indexSize,offsetSize);
		//printf("Index=%d\n", index_bits);
		
		int tag_bits = get_tag_bits(addr,indexSize,offsetSize, blockSize);
		//printf("Tag=%d\n",tag_bits);
		//decode
		// index and offset
		if ((id == '2' || id == '1' || id == '0') && splitFlag == 0){
			if (cache[index_bits].valid == 0){
				compulsoryMiss++;
				cache[index_bits].tag = tag_bits;
				cache[index_bits].valid = 1;
			}
			else if (cache[index_bits].valid != 0){
				if(cache[index_bits].tag == tag_bits )
					hit++;
				else {
					miss++;
					cache[index_bits].tag = tag_bits;
				}
			}
		}
		else if (id == '2' && splitFlag == 1){
			if (iCache[index_bits].valid == 0){
				iCompulsoryMiss++;
				iCache[index_bits].tag = tag_bits;
				iCache[index_bits].valid = 1;
			}
			else if (iCache[index_bits].valid != 0){
				if(iCache[index_bits].tag == tag_bits )
					hit++;
				else {
					imiss++;
					iCache[index_bits].tag = tag_bits;
				}
			}
		}

		else if (id == '1' || id == '0' && splitFlag == 1){
			if (dCache[index_bits].valid == 0){
				dCompulsoryMiss++;
				dCache[index_bits].tag = tag_bits;
				dCache[index_bits].valid = 1;
			}
			else if (dCache[index_bits].valid != 0){
				if(dCache[index_bits].tag == tag_bits )
					hit++;
				else {
					dmiss++;
					dCache[index_bits].tag = tag_bits;
				}
			}
		}

	}
		if(splitFlag){

		hitRate = (float)(((float)hit/(hit+miss+iCompulsoryMiss + dCompulsoryMiss + imiss +dmiss))*100);	
		printf("*****R E S U L T S  F O R  S P L I T  C A C H E*****\n");
		printf("Compulsory Miss Instruction = %d\n", iCompulsoryMiss);
		printf("Compulsory Miss Data        = %d\n", dCompulsoryMiss);
		//printf("Miss = %d\n", miss);
		printf("iMiss                       = %d\n", imiss);
		printf("dMiss                       = %d\n", dmiss);
		printf("Total Miss                  = %d\n", dmiss + iCompulsoryMiss + dCompulsoryMiss);
		printf("Hit                         = %d\n", hit);
		printf("Total iteration             = %d\n", hit+miss+iCompulsoryMiss + dCompulsoryMiss + imiss +dmiss);
		printf("HIT RATE                    = %.5f%%\n",hitRate);
		printf("MISS RATE                   = %.5f%%\n",(100-hitRate));
	}
	else{
		printf("*****R E S U L T S  F O R  U N I F I E D  C A C H E*****\n");
		printf("Compulsory Miss Instruction and Data = %d\n", compulsoryMiss);
		printf("Miss                                 = %d\n", miss);
		hitRate = (float)(((float)hit/(hit + miss+ compulsoryMiss))*100);	
		printf("Total Miss                           = %d\n", miss + compulsoryMiss);
		printf("Hit                                  = %d\n", hit);
		printf("Total iteration                      = %d\n", hit + miss+ compulsoryMiss);
		printf("HIT RATE                             = %.5f%%\n",hitRate);
		printf("MISS RATE                            = %.5f%%\n",(100-hitRate));
	}
fclose(tracefile);
return 0;
}


int setAssociative(int cacheSize, int blockSize, int splitFlag, int assocFlag){

	int offsetSize;
	int indexSize;
	int tagSize;
	int blockNumber;
	int i;
	char id;
	int requiredIndex;
	int flag = 0;
	unsigned int addr;
	int compulsoryMiss = 0;
	int miss = 0;
	int hit = 0;
	int dmiss = 0;
	int imiss = 0;
	int icompulsoryMiss = 0;
	int dcompulsoryMiss = 0;
	int counter = 0;
	float hitRate = 0;
	if (splitFlag)
	{
		cacheSize = cacheSize/2;
		blockNumber = (cacheSize/blockSize)/4;
	}
	else
		blockNumber = (cacheSize/blockSize)/4;

	Cache_t cache[blockNumber];
	Cache_t icache[blockNumber];
	Cache_t dcache[blockNumber];
	offsetSize = (int)((log(blockSize)/log(2)));
	indexSize = (int)((float)log(blockNumber)/log(2));
	
	//printf("Number of lines=%d\n", blockNumber);
	//printf("Offset is=%d\n", offsetSize);
	//printf("Index Size = %d\n", indexSize);

	
	if(!splitFlag){
		for(i = 0; i < blockNumber; ++i){
			for (int k = 0; k < 4; ++k)
			{
				cache[i].tag[k] = 0;
				cache[i].age[k] = 0;
				cache[i].valid[k] = 0;
			}
		}
	}
	else{
		for(i = 0; i < blockNumber; ++i){
			for (int k = 0; k < 4; ++k)
			{
				icache[i].tag[k] = 0;
				icache[i].age[k] = 0;
				icache[i].valid[k] = 0;
				dcache[i].tag[k] = 0;
				dcache[i].age[k] = 0;
				dcache[i].valid[k] = 0;
			}
		}

	}
FILE * tracefile; //pointer to file obj

tracefile = fopen ("trace.din","r");

if(tracefile == NULL){
	printf("Unable to open the trace file\n");
	return 0;
}
	//while(feof(tracefile)){
		char buffer[20];

		//fgets
		while(fgets(buffer,20, tracefile) != NULL){
		char *p;
		p = strtok(buffer," ");
		//printf("%s\n",p);	
		id = p[0];
		//printf("%c\n",id);
		char *pEnd;
		p = strtok(NULL, " ");
		if(p == NULL)
	    return 0;
		addr = (unsigned int)strtol(p, &pEnd , 16);
		//printf("Addr=%d\n", addr);
		int index_bits = get_index_bits(addr,indexSize,offsetSize);
		//printf("Index=%d\n", index_bits);
		
		int tag_bits = get_tag_bits(addr,indexSize,offsetSize,blockSize);
		//printf("Tag=%d\n",tag_bits);
		//decode
		// index and offset
		if ((id == '2' || id == '1' || id == '0') && splitFlag == 0){
				counter = 0;
				for (int l = 0; l < 4; ++l)
				{ 
					if (cache[index_bits].valid[l] == 0 && flag == 0){
						compulsoryMiss++;
						//printf("cm=%d \n",compulsoryMiss);
						//printf ("l = %d v = %d \n",l, cache[index_bits].valid[l]);
						cache[index_bits].valid[l] = 1;
						requiredIndex = l;
						flag = 1;
						break;
				}
					else if(cache[index_bits].tag[l] == tag_bits && cache[index_bits].valid[l] == 1){
						hit++;
						//printf("Hit\n");
						cache[index_bits].valid[l] = 1;
						flag = 2;
						break;
				}
					else if(cache[index_bits].tag[l] != tag_bits && cache[index_bits].valid[l] == 1){
					counter++;
					if(counter == 4){
						flag = 6;
						//miss++;
					}
				}
				
			
			}
				for(int j = 0;j < 4;j++){
						if(flag == 1 && j == requiredIndex){
							cache[index_bits].tag[j] = tag_bits;
							cache[index_bits].valid[j] = 1;
							cache[index_bits].age[j] = 4;
							//printf("age at %d =%d\n",j, cache[index_bits].age[j]);
							//printf("valid %d =%d\n",j, cache[index_bits].valid[j]);
							for (int k = 0; k < 4; ++k)
							{
								if(cache[index_bits].age[k] <= 4 && cache[index_bits].age[k] > 1 && j != k){
									cache[index_bits].age[k] = cache[index_bits].age[k] -1;
									//printf("age at %d =%d\n",k, cache[index_bits].age[k]);
								}
							}
							flag = 0;
							counter = 0;
						}
						 else if(cache[index_bits].tag[j] == tag_bits && flag == 2){
						// 	hit++;
							cache[index_bits].age[j] = 4;
						 	//printf("age at %d =%d\n",j, cache[index_bits].age[j]);
						 	flag = 0;
						 	for (int k = 0; k < 4; ++k)
							{
								if(cache[index_bits].age[k] <= 4 && cache[index_bits].age[k] > 1 && j != k){
									 cache[index_bits].age[k] = cache[index_bits].age[k]-1;
									//printf("age at %d =%d\n",k, cache[index_bits].age[k]);
								}
							}

						 }	
						 //counter = 0;					
					
				}

				if (flag == 6){
					counter = 0;
					flag = 0;
					
					for (int k = 0; k < 4; ++k)
					{
							if(cache[index_bits].age[k] == 1){
								miss++;
								cache[index_bits].age[k] = 4;
								//printf("age at %d =%d\n",k,cache[index_bits].age[k]);
								cache[index_bits].tag[k] = tag_bits;
								for (int j = 0; j < 4; ++j)
								{
								if(cache[index_bits].age[j] <= 4 && cache[index_bits].age[j] > 1 &&  j!= k){
									cache[index_bits].age[j] = cache[index_bits].age[j] -1;
									//printf("age at %d =%d\n",j, cache[index_bits].age[j]);
								}
							}
								break;
							}
					}
				}
				else{
					counter = 0;


				}
				flag = 0;

			
		}

		else if (id == '2' && splitFlag == 1){
				counter = 0;
				for (int l = 0; l < 4; ++l)
				{ 
					if (icache[index_bits].valid[l] == 0 && flag == 0){
						icompulsoryMiss++;
						//printf("cm=%d \n",icompulsoryMiss);
						//printf ("l = %d v = %d \n",l, cache[index_bits].valid[l]);
						icache[index_bits].valid[l] = 1;	
						requiredIndex = l;
						flag = 1;
						break;
				}
				else if(icache[index_bits].tag[l] == tag_bits && icache[index_bits].valid[l] == 1){
						hit++;
						//printf("Hit\n");
						icache[index_bits].valid[l] = 1;
						flag = 2;
						break;
				}
				else if(icache[index_bits].tag[l] != tag_bits && icache[index_bits].valid[l] == 1){
					counter++;
					if(counter == 4){
						flag = 6;
						//miss++;
					}
					
				}
				
			
			}
				for(int j = 0;j < 4;j++){
						if(flag == 1 && j == requiredIndex){
							icache[index_bits].tag[j] = tag_bits;
							icache[index_bits].valid[j] = 1;
							icache[index_bits].age[j] = 4;
							//printf("age at %d =%d\n",j, icache[index_bits].age[j]);
							//printf("valid %d =%d\n",j, icache[index_bits].valid[j]);
							for (int k = 0; k < 4; ++k)
							{
								if(icache[index_bits].age[k] <= 4 && icache[index_bits].age[k] > 1 && j != k){
									icache[index_bits].age[k] = icache[index_bits].age[k]-1;
									//printf("age at %d =%d\n",k, icache[index_bits].age[k]);
								}
							}
							flag = 0;

						}
						 else if(icache[index_bits].tag[j] == tag_bits && flag == 2){
						// 	hit++;
							icache[index_bits].age[j] = 4;
						 	//printf("age at %d =%d\n",j, icache[index_bits].age[j]);
						 	flag = 0;
						 	for (int k = 0; k < 4; ++k)
							{
								if(icache[index_bits].age[k] <= 4 && icache[index_bits].age[k] > 1 && j != k){
									 icache[index_bits].age[k] = icache[index_bits].age[k]-1;
									//printf("age at %d =%d\n",k, cache[index_bits].age[k]);
								}
							}

						 }						
					
				}

				if (flag == 6){
					counter = 0;
					
					for (int k = 0; k < 4; ++k)
					{
							if(icache[index_bits].age[k] == 1){
								imiss++;
								icache[index_bits].age[k] = 4;
								//printf("age at %d =%d\n",k,icache[index_bits].age[k]);
								icache[index_bits].tag[k] = tag_bits;
								for (int j = 0; j < 4; ++j)
								{
								if(icache[index_bits].age[j] <= 4 && icache[index_bits].age[j] > 1 &&  j!= k){
									icache[index_bits].age[j] = icache[index_bits].age[j]-1;
									//printf("age at %d =%d\n",j, icache[index_bits].age[j]);
								}
							}
								break;
							}
					}
				}
				else{
					counter = 0;


				}
				flag = 0;

		}

		else if ((id == '0' || id == '1') && splitFlag == 1){
				counter = 0;
				for (int l = 0; l < 4; ++l)
				{ 
					if (dcache[index_bits].valid[l] == 0 && flag == 0){
						dcompulsoryMiss++;
						//printf("cm=%d \n",compulsoryMiss);
						//printf ("l = %d v = %d \n",l, cache[index_bits].valid[l]);
						dcache[index_bits].valid[l] = 1;
						requiredIndex = l;
						flag = 1;
						break;
				}
				else if(dcache[index_bits].tag[l] == tag_bits && dcache[index_bits].valid[l] == 1){
						hit++;
						//printf("Hit\n");
						dcache[index_bits].valid[l] = 1;
						flag = 2;
						break;
				}
				else if(dcache[index_bits].tag[l] != tag_bits && dcache[index_bits].valid[l] == 1){
					counter++;
					if(counter == 4){
						flag = 6;
					
					}
					
				}
				
			
			}
				for(int j = 0;j < 4;j++){
						if(flag == 1 && j == requiredIndex){
							dcache[index_bits].tag[j] = tag_bits;
							dcache[index_bits].valid[j] = 1;
							dcache[index_bits].age[j] = 4;
							//printf("age at %d =%d\n",j, dcache[index_bits].age[j]);
							//printf("valid %d =%d\n",j, dcache[index_bits].valid[j]);
							for (int k = 0; k < 4; ++k)
							{
								if(dcache[index_bits].age[k] <= 4 && dcache[index_bits].age[k] > 1 && j != k){
									dcache[index_bits].age[k] = dcache[index_bits].age[k] -1;
									//printf("age at %d =%d\n",k, dcache[index_bits].age[k]);
								}
							}
							flag = 0;

						}
						 else if(dcache[index_bits].tag[j] == tag_bits && flag == 2){
						// 	hit++;
							dcache[index_bits].age[j] = 4;
						 	//printf("age at %d =%d\n",j, dcache[index_bits].age[j]);
						 	flag = 0;
						 	for (int k = 0; k < 4; ++k)
							{
								if(dcache[index_bits].age[k] <= 4 && dcache[index_bits].age[k] > 1 && j != k){
									dcache[index_bits].age[k] = dcache[index_bits].age[k] -1;
									//printf("age at %d =%d\n",k, cache[index_bits].age[k]);
								}
							}

						 }						
					
				}

				if (flag == 6){
					counter = 0;
					
					for (int k = 0; k < 4; ++k)
					{
							if(dcache[index_bits].age[k] == 1){
								dmiss++;
								dcache[index_bits].age[k] = 4;
								//printf("age at %d =%d\n",k,dcache[index_bits].age[k]);
								dcache[index_bits].tag[k] = tag_bits;
								for (int j = 0; j < 4; ++j)
								{
								if(dcache[index_bits].age[j] <= 4 && dcache[index_bits].age[j] > 1 &&  j!= k){
									dcache[index_bits].age[j] = dcache[index_bits].age[j]-1;
									//printf("age at %d =%d\n",j, dcache[index_bits].age[j]);
								}
							}
								break;
							}
					}
				}
				
				flag = 0;
				counter = 0;
		}

		memset(buffer, 0, 20);


	}
		
	fclose(tracefile);
		if(!splitFlag){
		hitRate = (float)(((float)hit/(hit+miss+compulsoryMiss))*100);	

		printf("*****R E S U L T S  F O R  U N I F I E D C A C H E  4 - W A Y  A S S O C I A T I V E ( %d )*****\n",blockSize);
		printf("Compulsory Miss = %d\n", compulsoryMiss); 
		printf("Miss            = %d\n", miss);
		printf("Total Miss      = %d\n", miss + compulsoryMiss);
		printf("Hit             = %d\n", hit);
		printf("Total iteration = %d\n", hit+miss+compulsoryMiss);
		printf("HIT RATE        = %.5f%%\n",hitRate);
		printf("MISS RATE       = %.5f%%\n",(100-hitRate));
	}
	else{
		hitRate = (float)(((float)hit/(hit + imiss + dmiss + dcompulsoryMiss + icompulsoryMiss))*100);	
		printf("*****R E S U L T S  F O R  S P L I T  C A C H E  4 - W A Y  A S S O C I A T I V E ( %d )*****\n",blockSize);
		printf("Compulsory Miss for Instruction = %d\n", icompulsoryMiss);
		printf("Compulsory Miss for Data        = %d\n", dcompulsoryMiss);
		printf("Total Miss                      = %d\n", imiss + dmiss + dcompulsoryMiss + icompulsoryMiss);
		printf("Hit                             = %d\n", hit);
		printf("Total iteration                 = %d\n", hit + imiss + dmiss + dcompulsoryMiss + icompulsoryMiss);
		printf("HIT RATE                        = %.5f%%\n",hitRate);
		printf("MISS RATE                       = %.5f%%\n",(100-hitRate));
	}

return 0;
}


int main(int argc, char *argv[]){

	int splitFlag;
	int cacheSize;
	int blockSize;
	int assocFlag;
		
	if (argc < 5){
	fprintf(stderr,"Usage:%s <cacheSize> <blockSize> <combinedShared 1:0> <associativity 1:4> \n", argv[0]);
        return 0;
	}
	cacheSize = atoi(argv[1]);
	blockSize = atoi(argv[2]);
	splitFlag = atoi(argv[3]);
	assocFlag = atoi(argv[4]);
	if(assocFlag == 1){
		directMapping(cacheSize,blockSize,splitFlag,assocFlag);
	}
	else if (assocFlag == 4){
		setAssociative(cacheSize,blockSize,splitFlag,assocFlag);
	}
	else
		fprintf(stderr,"Incorrect associativity\nUsage %s<cacheSize> <blockSize> <combinedShared 1:0> <associativity 1:4> \n", argv[0]);
	return 0;
}
	