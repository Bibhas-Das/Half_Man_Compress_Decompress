/* Program Name : Halfman compression
 * Programmer name : Bibhas Das
 * Start on : 11 OCT 2024
 * Finish on : 29 OCT 2024
 * */




#include <stdio.h>
#include <stdlib.h>

//Its for chractes occures
typedef struct node 
{
    char ch;
    int count;
} node;

typedef node *nodept;



//This is for creating tree
typedef struct tree
{
	int value;
	char character;
	struct tree *left;
	struct tree *right;
}tree;

typedef tree *treept;



//This is for encoding table of each character
typedef struct table
{
	char ch;
	char *code;
}en_table;






////////////////// DECLARACTION //////////////////////////
treept createTreeNode(char ch, int nu);
void stack_push(treept *stack, int *top, treept node);
treept stack_pop(treept *stack, int *top);
treept createTreeFromStack(treept *stack, int *top);
treept createTree(nodept node, int len);
nodept findUniqueCharacters(char *str);
void bubbleShort(nodept node, int len);
void traverseTree(treept parent);
void printUniqueCharacters(nodept nd); 

void assignCode(treept node, char *stack, int *top, int child, en_table *table, int *index);
void halfmanEncoding(char *filename, char *str, en_table *table, int table_len);
void setTopToCorrectPosition(int *top, char *stack);
long length(char *str);
char* desimalToBinary(long number);
long binaryToDesimal(char *str);
char* read(char *filename);
void compress(char *filename, char *outputfile);//This is actual function 
void increaseBitsCounter(char *countBits);
unsigned char binaryToByte(const char *binaryStr); 

en_table* createDocededTable(char *bitsArray,size_t table_len, size_t last_index);
void decodeWrite(char* bitsArray,size_t index,en_table *table,char *outputfile);
/////////////////////////////////////////////////////////


void decodeWrite(char *bitsArray,size_t index,en_table *table,char *outputfile)
{
    printf("\nDecode : ");

    //cheking all tables code if present from left to right of encoded bits
    //If present then just decode with orginal chracter
    
    //maintaing a stack
    char *code_stack = (char*)malloc(sizeof(char));
    int stack_index = 0;
	
    FILE *filept = fopen(outputfile,"wb");

    for(size_t i=0; i<index; i++)
    {
    	code_stack[stack_index++] = bitsArray[i];


	for(size_t j = 0; table[j].ch != '\0'; j++)
	{
		//if(check code_stack with (stack_index-1) == table[j].code)
		//write(table[j].ch);
		//stack_index = 0;
		//break;
		int lenCode = length(table[j].code);
		
		//printf("\nstack_index - lenCode : %d - %d",stack_index,lenCode);

		if((stack_index) == lenCode)
		{
			//printf("\nenter if");
			int flag = 1;
			for(int k = 0; k < stack_index; k++)
			{
				//printf("\nenter for cheking 1");
				if(code_stack[k] != table[j].code[k])
				{
					flag = 0;
					break;
				}
			}
			if(flag == 1)
			{
				//printf("\ntest");
				//char a;scanf("%c",&a);
				fwrite(&table[j].ch, sizeof(char), 1, filept);	
				//printf("%c",table[j].ch);
				stack_index = 0;
				break;
			}
		}

	}
	code_stack = (char*)realloc(code_stack,sizeof(char)*(stack_index+1));
    }

    fclose(filept);

}



en_table* createDocededTable(char *bitsArray,size_t table_len, size_t last_index)
{
	char ch_stack[9];
	int ch_index=0;

	char *code_stack = (char*)malloc(sizeof(char));
	int code_index=0;

	en_table *table = (en_table*)malloc(sizeof(en_table));
	int table_index = 0;

	
	
	size_t i = last_index - table_len;
	
	while(i<last_index)
	{
		size_t code_len=0;

		//find chracters from table
		for(int j=i; j < (i+8); j++)
		{
			ch_stack[ch_index++] = bitsArray[j];
			if(ch_index == 8)
			{
				ch_stack[8] = '\0';
				table[table_index].ch = binaryToDesimal(ch_stack);
				//printf("\nChracter [ %c ] ",binaryToDesimal(ch_stack));
				ch_index = 0;
			}
		}

		i += 8;
		
		//find codeword's length of previous chracters
		for(int j=i; j < (i+8); j++)
		{
			ch_stack[ch_index++] = bitsArray[j];
			if(ch_index == 8)
			{
				ch_stack[8] = '\0';
				code_len = binaryToDesimal(ch_stack);
				//printf("code length [ %ld ] ",code_len);
				ch_index = 0;
			}
		}

		i += 8;

		//find the actual codeword for the chracter
		for(int j=i; j < (i+code_len); j++)
		{
			code_stack[code_index++] = bitsArray[j];
			code_stack = (char*)realloc(code_stack,sizeof(char)*(code_index+1));
		}

		code_stack[code_index]='\0';
		
		//copping the codeword to table code varibale
		table[table_index].code = (char*)malloc(sizeof(char)*(code_index+1));

		for(int k = 0; k < code_index; k++)
			table[table_index].code[k] = code_stack[k];
		table[table_index++].code[code_index] = '\0';

		//printf("codeword : %s",code_stack);


		code_index = 0;

		i += code_len;

		table = (en_table*)realloc(table,sizeof(en_table) * (table_index + 1));
	
	
	}

	table[table_index].ch = '\0';
	table[table_index].code = NULL;
	
	return table;

}






void increaseBitsCounter(char *countBits)
{
	for(int i=31;i>=0;i--)
	{
		if(countBits[i]=='0')
		{
			countBits[i]='1';
			break;
		}
		else
		{
			countBits[i]='0';
			continue;
		}
	}	
}





char* read(char *filename)
{
	FILE *fptr;
	fptr=fopen(filename,"r");
	if(fptr==NULL)
		return NULL;

	char *string = (char*)malloc(sizeof(char));
	long index=0;

	char ch;
	while( (ch=fgetc(fptr))!= EOF )
	{
		string[index++]=ch;
		char *temp = (char*)realloc(string,sizeof(char)*index+1);
		if(!temp)
		{
			free(string);
			return NULL;
		}
		else
		{
			string=temp;
		}
	}
	fclose(fptr);
	string[index]='\0';

	return string;
}


unsigned char binaryToByte(const char *binaryStr) 
{
    //printf("\nThe binary string : %s\n",binaryStr);
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++) 
    {
        byte = (byte << 1) | (binaryStr[i] - '0');
    }
    return byte;
}



char* desimalToBinary(long number) 
{
    static char binaryString[9];
    binaryString[8] = '\0';

    for (int i = 7; i >= 0; i--) 
    {
        binaryString[i] = (number & 1) ? '1' : '0';
        number >>= 1;
    }

    return binaryString;
}

long binaryToDesimal(char *binaryString) 
{
    long len = length(binaryString);
    long desimal = 0;

    for (long i = 0; i < len; i++) 
    {
        desimal = desimal * 2 + (binaryString[i] - '0');
    }

    return desimal;
}


long length(char *str)// "-c\0"
{
    long len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}





void setTopToCorrectPosition(int *top, char *stack)
{
	*top = *top - 1;
	if(stack[*top - 1 ] == '0')
	{
		*top = *top - 1;
		return;
	}
	else
	{
		if(*top > 1 )
			setTopToCorrectPosition(top,stack);
	}
}



// 0 for left child and 1 for right child
void assignCode(treept root, char *stack, int *top, int child, en_table *_table, int *index)
{

	if(root==NULL)
		return;
	else
	{
		if(root->left == NULL && root->right == NULL)
		{
			char *temp = realloc(stack,sizeof(char) * (*top + 1));
			stack = temp;

			stack[*top]='\0';
			
			//copy the chracter to structure chracter varibale;
			_table[*index].ch  = root->character;
			

			//This is for copy the code to structure chracter pointer;
			_table[*index].code = (char*)malloc(sizeof(char));
			
			int i;
			for(i=0; stack[i]!='\0';i++)
			{
				_table[*index].code = (char*)realloc(_table[*index].code,sizeof(char) * i+1);
				_table[*index].code[i]=stack[i];
			}
		
			_table[*index].code = (char*)realloc(_table[*index].code,sizeof(char) * i+1);
			_table[*index].code[i]='\0';
			////////////////////////////


			*index = *index + 1;
			
			//printf("\nChracter : %c and Code : %s",_table[*index -1 ].ch, _table[*index -1].code);
	
			//pop //
			if(child == 0)
			{
				*top = (*top)-1;
			}
			else
			{
				
				//*top = (*top) - 2;
				//need to check this node has child value 0 or 1
				//if it is 0 then pop again finish
				//else pop again and repete whole precess
				
				//printf("\ntop : %d and stack : %s",*top,stack);

				setTopToCorrectPosition(top,stack);


			}
		}
		else
		{
			char *temp = realloc(stack,sizeof(char) * (*top + 1));
			stack = temp;

			stack[(*top)++] = '0';
			assignCode(root->left, stack, top, 0, _table, index);
			stack[(*top)++] = '1';
			assignCode(root->right, stack, top, 1, _table, index);
		}
	}
}





void stack_push(treept *stack, int *top, treept node)
{
	if(*top == -1)
	{
		stack[0]=node;
		*top=(*top)+1;
	}
	else
	{
		if(stack[*top]->value >= node->value)
		{
			stack[(*top)+1]=node;
			*top=(*top)+1;
		}
		else
		{
			int i=*top;
			while((stack[i]->value < node->value) && i > -1)
			{
				stack[i+1]=stack[i];
				i--;
			}
			stack[i+1]=node;
			*top=(*top)+1;
		}
	}
}


treept stack_pop(treept *stack, int *top)
{
	*top=(*top)-1;
	return stack[(*top)+1];
}


treept createTreeFromStack(treept *stack, int *top)
{
	if(*top==0)//only one node
		 return stack[*top];
	else
	{
		treept first = stack_pop(stack,top);
		treept second = stack_pop(stack,top);

		treept temp = createTreeNode('\0',first->value + second->value);
		temp->left = first;
		temp->right = second;

		stack_push(stack,top,temp);

		createTreeFromStack(stack, top);
	}
}






treept createTreeNode(char character,int value)
{
	treept new = (treept)malloc(sizeof(tree));
	new->character=character;
	new->value=value;
	new->left=NULL;
	new->right=NULL;
	return new;	
}


treept createTree(nodept node, int len)
{   
    //declear a stack that holds orphan parent nodes
    treept node_stack[(len/2)+1];    
    int stack_top = -1;


    int leng=0;
    if(len%2)
	    leng=len-1;
    else
	    leng=len;
    for(int i=0; i < leng; i+=2)
    {
    	treept firstNode  = createTreeNode(node[i].ch, node[i].count); 
	treept secondNode = createTreeNode(node[i+1].ch, node[i+1].count);

	if(stack_top == -1) //stack is empyty
	{
		treept tempTree = createTreeNode('\0', firstNode->value + secondNode->value);
		tempTree->left = firstNode;
		tempTree->right = secondNode;

		stack_push(node_stack, &stack_top, tempTree);
	}
	else
	{
		treept stackTopTree = stack_pop(node_stack, &stack_top);
		
		treept first = stackTopTree;
		treept second = firstNode; 
		treept third = secondNode;

		//find out 2 lowers value trees (top stack tree, and cureent two trees)
		if(firstNode->value < stackTopTree->value) 
		{
			second = stackTopTree;//4
			first = firstNode; 
		}
		if(secondNode->value < first->value)
		{
			third = second;
			second = first;
			first = secondNode;
		}
		if(secondNode->value < second->value)
		{
			third = second;
			second = secondNode;//4
		}
		
		stack_push(node_stack, &stack_top, third);
	
		
		treept temp = createTreeNode('\0', first->value + second->value);
		temp->left = first;
		temp->right = second;

		
		stack_push(node_stack, &stack_top, temp);
		 	
	}

    }//end of for

    if(len%2)
    {
    	//action for index node[len-1];
	treept temp = createTreeNode(node[len-1].ch, node[len-1].count);
	stack_push(node_stack, &stack_top, temp);

    }
    
    return createTreeFromStack(node_stack, &stack_top);

}





nodept findUniqueCharacters(char *str) 
{
    nodept nd = (nodept)malloc(sizeof(node));
    if (!nd) return NULL;

    nd[0].ch = '\0'; 
    nd[0].count = 1;

    int index = 0;

    for (int i = 0; str[i] != '\0'; i++) 
    {
        int flag = 0;

        for (int j = 0; j < index; j++) 
	{
            if (nd[j].ch == str[i]) 
	    {
                nd[j].count += 1;
                flag = 1; 
                break;
            }
        }

        if (!flag) 
	{
            if (index > 0) 
	    {
                nodept temp = (nodept)realloc(nd, sizeof(node) * (index + 1));
                if (!temp) 
		{
                    free(nd); 
                    return NULL; 
                }
                nd = temp;
            }

            nd[index].ch = str[i];
            nd[index].count = 1;
            index++;
        }
    }

    nodept temp = (nodept)realloc(nd, sizeof(node) * (index + 1)); 
    
    if (!temp) 
    {
        free(nd);
        return NULL; 
    }
    nd = temp;
    
    nd[index].ch = '\0'; 
    nd[index].count = 1; 

    return nd;
}






void bubbleShort(nodept node, int len)
{
	for(int index_i=0; index_i<len;index_i++ )
	{
		int flag=0;
		for(int index_j=0; index_j < len-index_i-1; index_j++)
		{
			if(node[index_j].count > node[index_j+1].count)
			{
				flag=1;
				//temp variable A to temp
				char ch = node[index_j].ch;
				int count = node[index_j].count; 

				//swap B to A
				node[index_j].ch = node[index_j+1].ch;
				node[index_j].count = node[index_j+1].count;

				//Finaly temp to B
				node[index_j+1].ch = ch;
				node[index_j+1].count = count;	
			}
		}
		if(flag==0)
			break;
	}
}




void traverseTree(treept parent)
{
	if(parent==NULL)
		return;
	printf("\nChracter : %c, Value : %d",parent->character,parent->value);
	printf("\n__Left Branch__");
	traverseTree(parent->left);
	printf("\n__Right Branch__");
	traverseTree(parent->right);
}



void printUniqueCharacters(nodept nd) 
{
    for (int i = 0; nd[i].ch != '\0'; i++) 
    {
        printf("Character: %c, Frequency: %d\n", nd[i].ch, nd[i].count);
    }
}



//                    output_file, text to encode,    table,     table length
void halfmanEncoding(char* filename, char *str, en_table *table, int table_len)
{
	//stack of 8 bits of codes
	char string_bytes[9];
	//stack top
	int top = 0;
	
	char countBits[32];

	unsigned char byte;

	for(int i=0;i<32;i++) countBits[i]='0';
	
	//increaseBitsCounter(countBits);


	FILE *fp = fopen(filename , "wb");
	if(fp==NULL)
		return;
	
	//printf("\n\nThe encoded text octated : ");
	//staring write teh encoded text into output file in form of chracter after convert 8 bits pair of bits into ascii chracters
	for(long i=0; str[i]!='\0';i++)
	{
		for(int j=0;j<table_len; j++)
		{
			if(table[j].ch==str[i])
			{
				char *code = table[j].code;
				for(int k=0; code[k]!='\0'; k++)
				{
					string_bytes[top++] = code[k];
					if(top == 8)
					{
						string_bytes[top] = '\0';
						
						//fprintf(fp,binaryToChar(string_bytes));
					        	
						byte = binaryToByte(string_bytes);
        					fwrite(&byte, sizeof(unsigned char), 1, fp);
						
						top=0;
						//printf("%s ",string_bytes);	
					}
				}	
				break;
			}


		}
	}//file test is written but there might be some extra bits those coundnot be pair of 8 bits


        //printf("\n[ * ] All chracters of file content is encoded and stored successfully");
	

	//store whole table to help of decoding
	for(int i=0; i<table_len;i++)
	{
		char *code = table[i].code;
		char *bin = desimalToBinary(table[i].ch);//character ki binary string
			
		//Alphabtets bianary
		for(int k=0;bin[k]!='\0';k++)
		{
			//count bits of tables
			increaseBitsCounter(countBits);

			string_bytes[top++]=bin[k];
			if(top==8)
			{
				string_bytes[top]='\0';

				//fprintf(fp,binaryToChar(string_bytes));
				byte = binaryToByte(string_bytes);
        			fwrite(&byte, sizeof(unsigned char), 1, fp);

				top=0;
			}
		}
	
		//length of code 8 bits are reserved(for length of code ward of chracters)
		char *lenbits = desimalToBinary(length(code));
		
		for(int k=0;lenbits[k]!='\0';k++)
		{
			//count bits of tables
			increaseBitsCounter(countBits);

			string_bytes[top++]=lenbits[k];
			if(top==8)
			{
				string_bytes[top]='\0';
				
				//fprintf(fp,binaryToChar(string_bytes));
				byte = binaryToByte(string_bytes);
        			fwrite(&byte, sizeof(unsigned char), 1, fp);

				top=0;
			}
		}

		//Then Codeward binary
		for(int k=0; code[k]!='\0';k++)
		{
			//count bits of tables
			increaseBitsCounter(countBits);

			string_bytes[top++]=code[k];
			if(top==8)
			{
				string_bytes[top]='\0';
				
				//fprintf(fp,binaryToChar(string_bytes));
				byte = binaryToByte(string_bytes);
        			fwrite(&byte, sizeof(unsigned char), 1, fp);

				top=0;
			}
		}

	}//whole table are written in same format 8 bits pair into characters. still extra bits might be left
	 

        //printf("\n[ * ] Whole table is encoded and stored successfully");
	
	//We need to store total table length in bianry (8 Bytes / 32 bits extra)
	
	for(int k=0; k<32; k++)
	{
		string_bytes[top++]=countBits[k];
		if(top==8)
		{
			//fprintf(fp, binaryToChar(string_bytes));
			byte = binaryToByte(string_bytes);
        		fwrite(&byte, sizeof(unsigned char), 1, fp);

			top=0;
		}
	}

	//this 2 line is for just print 32 bits string
	char ch = countBits[31];
	countBits[31]='\0';

        //printf("\n[ * ] The length of the table in bits (%s%c) is encoded and stored successfully",countBits,ch);

	
	
	//Extra bits to be padded
	int paddedBits = 8 - top;

	//printf("\nThe need of bits to make octate and top : %d",paddedBits);
	

	if(paddedBits)
	{
		//printf("\n\npadded bits length : %d and its binary : %s and its chracter %d",paddedBits,desimalToBinary(paddedBits),length(binaryToChar(desimalToBinary(paddedBits))));

		for(int k=0; k<paddedBits; k++)
		{
			string_bytes[top++]='0';
		}
		string_bytes[top]='\0';

		//printf("\nThe last byte include padding : %s and added bits are : %d",string_bytes,paddedBits);
		
		//bits with padding bits
		//fprintf(fp,binaryToChar(string_bytes));
		byte = binaryToByte(string_bytes);
        	fwrite(&byte, sizeof(unsigned char), 1, fp);

		
		//information how mant bits are padded in binary
		//fprintf(fp,binaryToChar(desimalToBinary(paddedBits)));
		byte = binaryToByte(desimalToBinary(paddedBits));
        	fwrite(&byte, sizeof(unsigned char), 1, fp);


		//printf("\n[ * ] Extra Bits are added for roundoff");
		//printf("\n[ * ] Last octate and added bit's length(8 bits) encoded and stored successfully");
	}

	fclose(fp);

}



void decompress(char *filename, char *outputfile)
{	
     //printf("\n__DECOMPRESS STARTED__");
	
    // Read each byte and print it in binary format
    
    FILE *file = fopen(filename, "rb");
    if (file == NULL) 
    {
        perror("Failed to open file");
        return;
    }


    unsigned char byte;
    size_t index = 0;

    char* bitsArray = (char*)malloc(sizeof(char));
   

    while (fread(&byte, sizeof(unsigned char), 1, file))
    {
	char *dec = desimalToBinary(byte);
	for(int i=0;dec[i]!='\0';i++)
	{
		bitsArray = (char*) realloc(bitsArray, sizeof(char) * (index + 1));
    		bitsArray[index++] = dec[i];
	}
    }

    //printf("\nThe last index of the bitArray is : %ld",index+1);
    

    //genarate the last octated for finding the padding bits length
    char padding_length[9];
    for(long i=index-8; i<index; i++)
    {
	    padding_length[i-(index-8)]=bitsArray[i];
	    //free((bitsArray+i));    
    }

    padding_length[8]='\0';
    //removed the padding length last octated
    index-=8;


 
    int padding_len = binaryToDesimal(padding_length);
   
    //removing the padded bits(1-7)
    index -= padding_len;

    //Extrating the 32 bits table length
    char table_length[33];
    for(long i=index-32; i<index; i++)
    {
    	table_length[i-(index-32)] = bitsArray[i];
	//free((bitsArray+i));
    }
    table_length[32]='\0';
    //remove the table length (32 bits)
    index -= 32;

    //printf("\nThe table length is 32 bits : %s",table_length);
	
    long table_len = binaryToDesimal(table_length);

    //printf("\nThe desimal of this value is : %ld",table_len);

    //get table from bits format
    en_table *table = createDocededTable(bitsArray,table_len,index);

    //removing encoded table's bits
    index -= table_len;

    //After provide tehe encoded text and table it will decode and write in a file
    decodeWrite(bitsArray,index,table,outputfile);

    fclose(file);
    //printf("\n[ * ] Decompression done");

}




void compress(char *filename, char *outputfile) 
{

    char *str = read(filename);

    long totalchar = length(str);

    //printf("[ * ] File content collected successfully full of %ld characters",totalchar);

    nodept _node = findUniqueCharacters(str);

    //printf("\n[ * ] Unique characters are identified");
    
    int len; 
    for(len=0; _node[len].ch!='\0';len++); // order of O(n) for get the length of teh array
   
    //printf("\n[ * ] Total unique characters are : %d",len);

    bubbleShort(_node,len);

    //printf("\n[ * ] All unique characters are shorted (Bubble short) by their frequencies");
 
    //printf("\n[ * ] Printing unique characters..\n");
   
    //printUniqueCharacters(_node);

    //printf("\n[ * ] Halfman tree is creating ...");
    
    treept root = createTree(_node,len);

    //printf("\n[ * ] Halfman tree is created based or chracter and their frequencies");

    //traverseTree(root); 
	
    //printf("length : %d",len);    
    
    char *char_stack = (char*)malloc(sizeof(char));// Issue fixed on this line on 31 oct 2024;  
						   // issue: this variable does not assined memory but in assignCode function it is re allocated so there is a probelm 
    int top=0,index=0;

    //en_table encodingTable[len];
    en_table *encodingTable = (en_table*)malloc(sizeof(en_table)*len);

    //printf("\n[ * ] Tree is traversing and assigning paths to each character");

    assignCode(root,char_stack, &top, 0, encodingTable , &index);
  
    //printf("\n[ * ] Tree is traversed successfully and collect all codewards");
       
    //for(int i=0; i<len; i++) printf("\nCharacter : %c and Code : %s",encodingTable[i].ch,encodingTable[i].code);
    
    //printf("\n[ * ] The whole file content is encoding based on encoding table...");
 
    halfmanEncoding(outputfile, str, encodingTable, len);

    //printf("\n[ DONE ] Compression DONE, Output file: %s\n",outputfile);
}






void main(int args, char **argv)
{
	if(args == 2)
	{
		char *filename = argv[1];

		int file_len = length(filename);
		
		if(file_len > 4 && filename[file_len-1]=='p' && filename[file_len-2]=='m' && filename[file_len-3]=='c' && filename[file_len-4]=='.')
		{
			char *output = (char*)malloc(sizeof(char) * (file_len - 3));
			int i;
			for(i=0; i < file_len - 4; i++)
				output[i] = filename[i];
			output[i]='\0';
			
			decompress(filename,output);

			printf("\n\tYour encoded file(%s) is decoded to (%s)\n\n",filename,output);
		}
		else
		{
			char *output = (char*)malloc(sizeof(char) * (file_len+5));
			int index = 0;
			for(int i = 0; i < file_len; i++)
				output[index++] = filename[i];
			
			char *extention = ".cmp";
			for(int i=0; extention[i]!='\0'; i++)
				output[index++] = extention[i];

			compress(filename,output);

			printf("\n\tYour file (%s) is encoded successfully to (%s)\n\n",filename,output);
		}
		
	}
	else
	{
		//error message
		printf("\n\tProgram name : Halfman Compression\n\tProgrammer : Bibhas Das\n\tLast Update : 29 oct,2024\n\n\tPlease use command %s <filename> [ENTER]\n\n",argv[0]);
		printf("\tExample");
		printf("\n\tFor encode : %s test.txt     [ENTER]",argv[0]);
		printf("\n\tFor decode : %s test.txt.cmp [ENTER]\n\n",argv[0]);
		printf("\tWarning\n\tThis is a smaple model. In this model too many memory space needed\n\tfor decoding case atleast (encoded file size) * 8 times space in \n\tbytes. and more than 256 bytes extra along with normal filesize \n\tduring encoding case. So be aware about your computer's memory space.\n\tBut it can be fixed without using this much space wastage, that \n\tupgradeed model will commimg soon.\n\n");

	}

}
