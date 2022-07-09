#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#define FNSIZE 50
#define R 31
#define M 997

typedef struct{
	char *kelime;
	char **locations;
}word;

typedef struct{
	word * words;
	int tableSize;
}hashTable;
/* PROTOTYPES and declerations: the explanation is under the main function:*/
unsigned long long int horner(const char* str);
int h1(unsigned long long int key);
int h2(unsigned long long int key);
int hash(unsigned long long int key,int i);
void insertToTable(hashTable * table,char* wrd,char* filename);
int searchTable(hashTable * table,char* wrd,int *numberOfIteration);
void searchForUserWord(hashTable * table,char* wrd);
int updateTable(hashTable * table,char * filename);
hashTable* init_table(hashTable * table);
bool isFull(hashTable * table);
word * createWord();
char * upper(char *str);
void writeTableToFile(hashTable *table);
void print(hashTable * table);
bool loadTable(hashTable * table);
int main(){
	int option;
	float loadFactor;
	char baseFile[100],takenWord[100];//baseFile will store the nameOfFile to be inserted
	hashTable * table = NULL;
	table =init_table(table);//initialization
	if(loadTable(table)){//if Dictionary.txt exists then load it
		puts("\t\t--> Your last insertions are loaded into the memory");
		puts("\t\t--> Please Press any key to start the program");
		getch();
    	system("cls");
	}
    while(true){
        printf("(1) --> Insert New File.\n(2) --> Look up a Word.\n(3) --> Print Dictionary.\n(4) --> Clear screen.\n(999) --> Exit.\nPlease Enter an option :");
        scanf("%d",&option);
        switch(option){
            case 1 :
                printf("Enter the new file's name : ");
                scanf("\n%s",baseFile);
                updateTable(table,baseFile);
                writeTableToFile(table);
                break;
            case 2 :
                printf("Enter the Word : ");
                scanf("\n%s",takenWord);        	
                searchForUserWord(table,upper(takenWord)); 
                break;
            case 3 :
                print(table);
                puts("Press any key to continue the program:");
                getch();
                system("CLS");
                break;
            case 4 :
                system("CLS");
                break;
            case 999 :
            	system("CLS");
            	printf("Table size: %d\n",table->tableSize);
				printf("%d Used from %d\nLoad factor: %f\n",table->tableSize,M,((float)table->tableSize)/(float)M);
                return 0;
            default :
                printf("Please Enter a valid option..\nPress Any key to continue...\n");
                getch();
                break;
        }
        printf("-----------------\n");
    }
	return 0;
}

/*

					In My explanation i might refer to my hash table with "dictionary"
					or "table",Both are the same.


*/
/*
Horner algorithm's result may be a massive number, so in order to handle
this problem i used unsigned long long integer type
and befor passing the string to the function i will convert it to capital case
since my search function is not case sensetive
Why capital not lower case? because capital letters have smaller corresponding
integers in ASCII table
*/
int h1(unsigned long long int key) {
   return key % M;
}
int h2(unsigned long long int key) {
   return 1 + (key % M-1);
}
int hash(unsigned long long int key, int i) {
   return (h1(key) + i * h2(key)) % M;
}
unsigned long long int horner(const char* str){
	unsigned long long int key=0;
	int i=0;
	
	while(str[i]!='\0'){
		key+=(str[i])*pow(R,strlen(str)-i-1);
		i++;
	}
	return key;
}
char * upper(char *str){
	int i=0;
	while(str[i]!='\0'){
		if(str[i]>96&&str[i]<123)
			str[i]=str[i]-32;	
		i++;
	}
	return str;
}
/*-----------------------------------*/
//have I filled the entire table?
// if yes return 1
bool isFull(hashTable * table){
	return (table->tableSize==M);
}
/*-----------------------------------*/
/* Initializing my hash table */
hashTable* init_table(hashTable * table){
	int i;
	word *temp;
	table = (hashTable*)calloc(M,sizeof(hashTable)); 
	table->tableSize=0;
	for(i=0;i<M;i++){
		table[i].words=createWord();
	}

    return table;
}
/*-----------------------------------*/
/* Allocating memory for word struct: INITIALIZATION*/
word * createWord(){
	word *temp;
	temp = (word*)malloc(sizeof(word));
	temp->kelime = (char*)calloc(50,sizeof(char));
	temp->kelime[0]='\0';
	temp->locations = (char**)malloc(FNSIZE*sizeof(char*));
	int i;
	for(i=0;i<FNSIZE;i++){
		temp->locations[i]=(char*)calloc(50,sizeof(char));
		temp->locations[i][0]='\0';	
	}
	
	return temp;
}
/*-----------------------------------*/

/*
Inserting new words to the dictionary, in case it found the word 
in our table it will add the file it came from to the word locations
(if it is a new location)
*/
void  insertToTable(hashTable * table,char* wrd,char* filename){
	int i=0,index;
	assert(isFull(table) == false);
	index =searchTable(table,wrd,&i);//if it found the word , thwn return its index,other wise -1
	if(index!=-1){//if the world is found in the table then add its location and return
		i=0;
		while(table[index].words->locations[i][0]!='\0'){
			if(strcmp(table[index].words->locations[i],filename)==0){
				return;
			}
			i++;
		}
		strcpy(table[index].words->locations[i],filename);
	}else{ // it is a new world , then add it to the table
		i=0;
		index = hash(horner(wrd),i);
		while(i<M && table[index].words->kelime[0] != '\0'){
			i++;
			index = hash(horner(wrd),i);
		}
		strcpy(table[index].words->kelime,wrd);
		strcpy(table[index].words->locations[0],filename);
		table->tableSize++;	
	}
}


/*
if it found the word , thwn return its index,other wise -1
*/
int searchTable(hashTable * table,char* wrd,int *numberOfIteration){
	int i =0;
	int index = hash(horner(wrd),i);
	while(( table[index].words->kelime[0] !='\0' && strcmp(table[index].words->kelime,wrd)!=0 )){
		i++;
		(*numberOfIteration)++;
		index=hash(horner(wrd),i);
	}
	if(strcmp(table[index].words->kelime,wrd)==0){
		return index;
	}
	return -1;
}

/*
In this function I am reading the file to be inserted word by word
and then inserting every word into the dictionary
*/
int updateTable(hashTable * table,char * filename){
	int i,flag=0;char c,word[50];// flag var is to protect the program from displaying the worning more than once
	//c char is to receive every char from the file,
	//word[50] is to store the word after it has been read char by char
	FILE * fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr,"could not open this file %s !\n",filename);
		return 0;
	}
	i=0;
	while(((c = fgetc(fp)) != EOF)){
		if((c >= 65 && c<=90) || (c>=97 && c <=122 )){
			word[i]=c;
			word[i+1] = '\0';
			i++;
		}else if(c == ' ' || c == '\n'){
			if(!isFull(table)){
				if(table->tableSize >= M*0.8 && !flag){
					printf("WORNING:\nYou have used 80% of the table Size\n");
					flag=1;
				}
				insertToTable(table,upper(word),filename);
			}
			i=0;
		}
	}
	
	fclose(fp);
	return 1;
}

/*
searching for the words entered by the user
if it is found then display its locations
if not then tell the user that
*/

void searchForUserWord(hashTable * table,char* wrd){
	int i,numberOfIteration=0;
	assert(table->tableSize != 0);//we can look up a word only if we have one word
	int	index =searchTable(table,wrd,&numberOfIteration);
	if(index!=-1){
		i=0;
		printf("%s is found in these documents:\n",wrd);
		while(table[index].words->locations[i][0]!='\0'){
			printf("%3s\t",table[index].words->locations[i]);
			i++;
		}
		printf("\nnumber of iterations:%d\n",numberOfIteration);
	}else{
		printf("\nnumber of iterations:%d\n",numberOfIteration);
		printf("This word is not found in the dictionary\n");	
	}
}

/*
printing the table 
*/
void print(hashTable * table){
	int i,j;
	for(i=0;i<M;i++){
		if (table[i].words->kelime[0] != '\0'){
			printf("{\n\t\"Index\":\"%d\",\n\t\"Word\":\"%3s\",\n\t\"Locations\":",i,table[i].words->kelime);
			for(j=0;j<FNSIZE;j++){
				if(table[i].words->locations[j][0]!='\0'&&table[i].words->locations[j+1][0]!='\0')
					printf("\"%1s\",",table[i].words->locations[j]);
				else if(table[i].words->locations[j][0]!='\0'&&table[i].words->locations[j+1][0]=='\0')
					printf("\"%1s\"",table[i].words->locations[j]);		
			}
			printf("\n}\n");
		}
	}	
}


/*
Writing the table to file ;
i tried tho store the data in an understandable form
so when someone opens the file he can understand what is inside
{
	index:
	word:
	locations:
}
data will be written to Dictionary.txt
*/
void writeTableToFile(hashTable *table){
    FILE* data;
    fopen("Dictionary.txt", "w");
    if ( (data=fopen("Dictionary.txt", "w")) == NULL )
    {
        printf("Error opening file\n");
        return ;   
    }
    int i,j;
	for(i=0;i<M;i++){
		if (table[i].words->kelime[0] != '\0'){
			fprintf(data,"{\n\tIndex:%d,\n\tWord:%s,\n\tLocations:",i,table[i].words->kelime);
			for(j=0;j<FNSIZE;j++){
				if(table[i].words->locations[j][0]!='\0' && table[i].words->locations[j+1][0]!='\0')
					fprintf(data,"%s,",table[i].words->locations[j]);
				else
					fprintf(data,"%s",table[i].words->locations[j]);
			}
		    fprintf(data,"\n}\n");	
		}
	}

    fclose(data);
	
}

/*
if the Dictionary.txt is found then load it and return true
otherwise,return false
*/
bool loadTable(hashTable * table){
	FILE * data;
	int index,flag=0; // flag var is to protect the program from displaying the worning more than once
    if (  (data=fopen("Dictionary.txt", "r")) == NULL )
    {
        return false;   
    }
    int j;char buffer[200],*wrd=NULL;
    while(!feof(data)){
    	j=0;
    	fscanf(data,"{\n\tIndex:%d,\n\t",&index);
    	fscanf(data,"Word:%[^,],\n\t",table[index].words->kelime);
		fscanf(data,"Locations:\t%s\n}\n",buffer);
		wrd = strtok(buffer,",");
		while(wrd != NULL){
			strcpy(table[index].words->locations[j],wrd);
			j++;
			wrd = strtok(NULL,",");
		}
		table->tableSize++;
		if(table->tableSize >= M*0.8 && !flag){
			printf("WORNING:\nYou have already used 0.80 of the table Size\n");
			flag=1;
		}
	}
	return true;
}

