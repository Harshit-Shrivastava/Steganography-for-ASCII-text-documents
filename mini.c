#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define hash_size 256
#define max 2050 //max allowed size of secret message in bits=2048 bits = 256 bytes

struct Node{
char ame[64]; //american word 
char bri[64]; //british word
int col; //whether node is 0-1 (straight=0) or a 1-0 (reversed copy=1) of an existing node 1-0
struct Node* next;
};

typedef struct Node* node; //otherwise struct is necessary

//function prototypes

node search(char*);
int calchash(char buf[]);
node getnode();
void addtotable(char*, int, char*, int);
void testlist();
void embed();
void createtable();
void to_bin(unsigned int n);
int input(); //converts encrypted text to binary and saves it in secret[]
void encrypt(char*); //compress and encrypt the message
void compress();

node hash[256]; 
char password[16];
char secret[max];
int secretlen=0;
int secretpos=0; //current secret bit to be embedded into the cover.txt file

static int calls=0;

int calchash(char buf[]){
int val=0,pos=0;

while(buf[pos]!='\0'){
val+=(int)buf[pos];
pos++;
}
 return val%hash_size;
}//calchash close

node getnode(){
return (node)malloc(sizeof(struct Node));
} //getnode() close


void to_binstore(unsigned int n)
{
if (n>1) to_binstore(n/2);
secret[secretpos]=(n%2)+48;
secretpos++;
}

void to_bin(unsigned int n)
{
	calls++;
if (n>1) to_bin (n/2);
}

int input(){
	
	int i=0;
	char c;
	
	FILE* f1=fopen("encrypted.txt","r"); //secret text
	
	c=getc(f1);
	
	while(c!=EOF){
	
	calls=0;	
	to_bin((int)c);
	
	
	for(i=8;i>calls;i--){
		secret[secretpos]='0';
		secretpos++;
     }
    
    to_binstore((int)c);
	
	calls=0;
	
	c=getc(f1);	
	}//while(c!=EOF) close
	
	
	// adding delimiter
	calls=0;
	to_bin(37);
	for(i=8;i>calls;i--){
		secret[secretpos]='0';
		secretpos++;
     }
    to_binstore(37);
	
	printf("\n");
	
	for(i=8;i>calls;i--){
		secret[secretpos]='0';		
		secretpos++;
	}
	to_binstore(37);
	
	secret[secretpos]='\0';
	
	fclose(f1);
	
	return secretpos;
	
}//input() close

void addtotable(char buf1[], int pos1, char buf2[], int pos2){

node wptr1=hash[pos1];

node p=getnode();

strcpy(p->ame,buf1);
strcpy(p->bri,buf2);
p->col=0;
p->next=NULL;

if(wptr1==NULL) hash[pos1]=p;

else{

node temp=hash[pos1];
while(temp->next!=NULL) temp=temp->next;

temp->next=p;
}

//do the same for the second string

node wptr2=hash[pos2];

p=getnode();

strcpy(p->ame,buf2);
strcpy(p->bri,buf1);
p->col=1;
p->next=NULL;

if(wptr2==NULL) hash[pos2]=p;

else{

node temp=hash[pos2];
while(temp->next!=NULL) temp=temp->next;

temp->next=p;
}

}//addtotable() close


void testlist(){

int i;

for(i=0;i<256;i++){

if(hash[i]!=NULL){

node p=hash[i];

while(p!=NULL){

printf("%d: %s %s %d\t",i,p->ame,p->bri,p->col);

p=p->next;
}
printf("\n");
}

}
}//testlist() close


node search(char buf[]){
 
 int index=calchash(buf); //index in hash table for the current word
    
 node temp=hash[index];
  
 if(temp==NULL) return NULL; 
 
 while(temp!=NULL){
	 
	 if(strcasecmp(temp->ame,buf)==0) return temp;
	 else temp=temp->next;
 }
 
 return temp; 
}//search() close

void embed(){

int match=0; //number of word matches in basecover.txt
int k=0, curlen=0; //initialization //curlen counts the number of words matched
char c,buf[64],pos=0;

	FILE* f1=fopen("basecover.txt","r"); //initial cover text
    
	FILE* f2=fopen("cover.txt","w"); //modified cover text to be sent to the other side

    c=getc(f1);
     
    while(c!=EOF){
 
     if(curlen>=secretlen){ //curlen=number of bits matched
        while(c!=EOF){
        putc(c,f2);
        c=getc(f1);}
        return;    
		 //write remaining contents of basecover.txt from this point onwards to cover.txt
	 }

    pos=0;
    
    while((c==' ' || c=='\n' ||  c=='\t') && c!= EOF){
		 putc(c,f2); //to include spaces, tabs or new lines in cover.txt
    c=getc(f1);
    }
    
    while(c!=' ' && c!='\n' &&  c!='\t' && c!= EOF){ //used to fetch a word from basecover.txt
    //if(buf[pos]!=',' || buf[pos]!='.')
    buf[pos++]=c;
    
    c=getc(f1);
  }
  
    buf[pos]='\0';
 
    node pairword=search(buf); //pairword is null if word is not found in the list
                              // else pairword contains the word to substitute in place of buf in cover.txt
 
    if(pairword!=NULL){
	match++;
	    
	    int colno=secret[curlen]-48;
	    int colfromword=pairword->col;
	    
	    if(colno==colfromword){
		k=0;
		while(pairword->ame[k]!='\0') putc(pairword->ame[k++],f2);
		//write pairword->ame to cover.txt
		}
	    
	    else{
			k=0;
			while(pairword->bri[k]!='\0') putc(pairword->bri[k++],f2);
		//write pairword->bri to cover.txt
		}
	    
		curlen++;		
	} //if(pairword!=NULL) close   
	
	else{
		k=0;
		while(buf[k]!='\0') putc(buf[k++],f2); //write unmatched word to cover.txt
	}

}//while(c!=EOF) close      
printf("%s%d%s","#matches=",match,"\n");

fclose(f2);
fclose(f1);

}//embed() close 


void createtable(){
   
char buf1[64],buf2[64],c;
int i,r,pos1=0,pos2=0;

for(i=0;i<256;i++) hash[i]=NULL;

FILE* f1=fopen("table.txt","r");

c=getc(f1);

while(c!=EOF){
  
  pos1=pos2=0; //to push a new word into buf
  
  while(c!=' ' && c!='\n'&& c!= EOF){ //used to fetch a word from table file
    buf1[pos1++]=c;
    c=getc(f1);
  }
  buf1[pos1]='\0';
  
  c=getc(f1);
  
  while(c!=' ' && c!='\n' && c!= EOF){ //used to fetch a word from table file
  buf2[pos2++]=c;
  c=getc(f1);
  }
  
  buf2[pos2]='\0';
   
  int val1=calchash(buf1);
  //printf("%d",val1);
  int val2=calchash(buf2);
  //printf("%d",val2);

  addtotable(buf1,val1,buf2,val2);
  
  c=getc(f1);
  
}//read until end of table file

}//createtable() close

void encrypt(char* pass){
	
	int i;
	
	char initcomm[]="openssl enc -aes-256-cbc -a -salt -in compressed.txt.gz -out encrypted.txt -pass pass:";
	char command[1024];
	
	memset(command,'\0',1024); //prevent garbage during string handling
	
	strcat(command,initcomm);
	strcat(command,pass);

    int ret=system(command);
	
	if (ret!=0)
	printf("%c","Error while encrypting the message. Exiting \n");
	
}//encrypt() close


void compress(){
    int ret;
    	
	ret=system("gzip -c secret.txt -9 > compressed.txt.gz");
	
	if (ret!=0)
	printf("%c","Error while compressing the message. Exiting \n");
	
}

int main(int argc, char* argv[]){

                   //argv[1] is the key (password)
if(argc!=2){
printf("%s","Usage: <executable name> <password> \n");
exit(1);
}

int i;

createtable();

compress();

encrypt(argv[1]);

secretlen=input();

printf("secretlen=%d bits\n",secretlen);

for(i=0;i<secretlen;i++){
	printf("%c",secret[i]);
	}

embed();

printf("\n");

/*
 * hash value will change if comma or fullstop is a part of the word! (change the search function)
 * in extraction, two words are equal even if they are delimited by comma or a fullstop!
 */

//testlist();

return 1;
}//main close
