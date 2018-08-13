#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>

//options of connection to Database
#define host "127.0.0.1"
#define port 0
#define user "root"
#define pass ""
#define database "bank"

//struct to store all informations on client
struct Client{
	char* fullname;
	char* email;
	char* password;
	char* pin;
	float balance;
};

//all functions used in program Bank
void mainMenu();
void connectionDB(MYSQL **);
void createAccount();
void compteBank();
void debit(MYSQL**,char *,float );
void credit(MYSQL**,char *,float );
char* concatString(char*, char*);
void clearBuffer();


//function main
int main(void){
	mainMenu();
	return 0;
}

//function mainMenu : display the interface of the bank options
void mainMenu(){
	unsigned short int option;

	system("clear");
	printf("\t\t\t\t welcome to the Islamic bank\n\n");
	printf("Islamic Banking is a banking system that is based on the principles of Islamic law (Sharia law).\n\n");
	printf("Our service :\n");
	printf(" 1 - Identify to your account\n");
	printf(" 2 - Create a new account\n");
	printf(" 0 - Exit\n");

	rightChoose:

	printf("\nPlease select the service type : ");
	scanf("%hu",&option);
	clearBuffer();

	switch(option){
		case 1:
			system("clear");
			compteBank();
			system("clear");
			mainMenu();
			break;
		case 2:
			system("clear");
			createAccount();
			sleep(5);
			system("clear");
			mainMenu();
			break;
		case 0:
			printf("\nThank you, We hope to see you again on our bank.\n\n");
			break;
		default:
			printf("\nError: please choose a valid option\n");
			goto rightChoose;
			break;
	}
}

void connectionDB(MYSQL **connection){

	//initialize object MYSQL
	if((*connection=mysql_init(NULL)) == NULL){
		printf("%s",mysql_error(*connection));
		exit(0);
	}

	//set options
	mysql_options(*connection,MYSQL_READ_DEFAULT_GROUP,"options");

	//set options of connections
	if(mysql_real_connect(*connection,host,user,pass,database,port,0,0) == NULL){
		printf("%s",mysql_error(*connection));
		exit(0);
	}
}

void createAccount(){
	struct Client *client;

	client = (struct Client *)malloc((sizeof(struct Client *)));

	client->fullname = (char*)malloc(sizeof(char*)*40);
	client->email = (char*)malloc(sizeof(char*)*50);
	client->password = (char*)malloc(sizeof(char*)*17);
	client->pin = (char*)malloc(sizeof(char*)*7);

	printf("Enter your fullname : ");
	scanf("%40[^\n]s",client->fullname);
	clearBuffer();

	printf("Enter your email : ");
	scanf("%50[^\n]s",client->email);
	clearBuffer();

	printf("Enter your password (prefer to use a password between 8-16 characters) : ");
	scanf("%17[^\n]s",client->password);
	clearBuffer();

	printf("Enter your pin (Must be 6 digits) : ");
	scanf("%7s",client->pin);
	clearBuffer();

	//create object MYSQL
	MYSQL *connection;
	connectionDB(&connection);

	char* query = "insert into client (fullname,email,password,pin,balance) values ('";
	query = concatString(query,client->fullname);
	query = concatString(query,"','");
	query = concatString(query,client->email);
	query = concatString(query,"','");
	query = concatString(query,client->password);
	query = concatString(query,"','");
	query = concatString(query,client->pin);
	query = concatString(query,"','0.00')");

	if(mysql_query(connection,query) == 0){
		printf("\nAccount created successfully, you will be redirect to mainMENU after 5 seconds\n\n");
	}else{
		printf("error : %s",mysql_error(connection));
		exit(0);
	}

	
	//free all pointers
	free(client);
	client=NULL;

	free(query);
	query=NULL;

	connection=NULL;
	mysql_close(connection);

	//to clear buffer after use function sleep
	fflush(stdout);
}

void compteBank(){
	struct Client *client;

	client = (struct Client *)malloc((sizeof(struct Client *)));

	client->fullname = (char*)malloc(sizeof(char)*40);
	client->email = (char*)malloc(sizeof(char)*50);
	client->password = (char*)malloc(sizeof(char)*17);
	client->pin = (char*)malloc(sizeof(char)*7);

	printf("Please enter your email and password to verify your identity\n\n");

	printf("Enter your email : ");
	scanf("%50[^\n]s",client->email);
	clearBuffer();

	printf("Enter your password : ");
	scanf("%16[^\n]s",client->password);
	clearBuffer();

	printf("\nwait to check your informations\n\n");

	//check if the user exist or not
	char *query = "select * from client where email='";
	query=concatString(query,client->email);
	query=concatString(query,"' and password='");
	query=concatString(query,client->password);
	query=concatString(query,"'");

	//create object MYSQL
	MYSQL *connection;
	connectionDB(&connection);

	MYSQL_RES *result;
	MYSQL_ROW row;

	if(mysql_query(connection,query) == 0){
		if((result = mysql_store_result(connection)) != NULL){
			if(mysql_num_rows(result) >= 1){
				row = mysql_fetch_row(result);

				client->fullname=row[1];
				strcpy(client->email,row[2]);
				strcpy(client->password,row[3]);
				strcpy(client->pin,row[4]);

				client->balance =strtod(row[5],NULL); //strtod => to convert string to float

				printf("Welcome to you account %s\n",client->fullname);
				printf("Balance : %f\n",client->balance);

				unsigned short int option;

				valideOption:
				printf("\n------ service ------\n");
				printf(" 1 - to Debit\n");
				printf(" 2 - to Credit\n");
				printf(" 0 - to Exit\n");

				printf("\nPlease select the service type : ");
				scanf("%hu",&option);
				clearBuffer();

				char pin[7];

				switch(option){
				case 1:
					validPinDebit:
					printf("Please insert your pin : ");
					scanf("%6[^\n]s",pin);
					clearBuffer();

					if(strcmp(client->pin,pin) == 0)
						debit(&connection,client->email,client->balance);
					else{
						printf("\nInvalid PIN numbers try again\n");
						goto validPinDebit;
					}
					break;
				case 2:
					validPinCredit:
					printf("Please insert your pin : ");
					scanf("%6[^\n]s",pin);
					clearBuffer();

					if(strcmp(client->pin,pin) == 0)
						credit(&connection,client->email,client->balance);
					else{
						printf("\nInvalid PIN numbers try again\n");
						goto validPinCredit;
					}
					break;
				case 0:
					break;
				default :
					goto valideOption;
					break;
				}
			}else{
				printf("Error : invalid Information\n\n");
			}
			mysql_free_result(result);
		}else{
			printf("Error : %s\n", mysql_error(connection));
		}
	}else{
		printf("Error: %s",mysql_error(connection));
	}

	
	//free all pointers
	free(client);
	client=NULL;

	free(query);
	query=NULL;

	connection=NULL;
	mysql_close(connection);

	printf("\nClick to Enter to Exit\n");
	clearBuffer();
	getchar();
}

void debit(MYSQL **connection,char* email,float oldBalance){
	float debit;
	printf("Enter your debit balance : ");
	scanf("%f",&debit);

	if(oldBalance - debit < 0){
		printf("You cannot debit balance, you have just %f in your balance, insert a credit\n",oldBalance);
		//credit(&connection,email,oldBalance);
	}else{
		char newBalance[10];
		gcvt(oldBalance - debit, 10, newBalance);

		char* query = "update client set balance=";
		query=concatString(query,newBalance);
		query=concatString(query," where email = ");
		query=concatString(query,"'");
		query=concatString(query,email);
		query=concatString(query,"'");
		if(mysql_query(*connection,query) == 0){
			printf("\nOperation Confirmed : debit\n");
			printf("your new balance is : %s\n",newBalance);
		}else{
			printf("%s\n",mysql_error(*connection));
		}
		free(query);
		query=NULL;
	}
}

void credit(MYSQL **connection,char* email,float oldBalance){
	float credit;
	printf("Enter your credit balance : ");
	scanf("%f",&credit);

	char newBalance[10];
	gcvt(credit+oldBalance, 10, newBalance);

	char* query = "update client set balance=";
	query=concatString(query,newBalance);
	query=concatString(query," where email = ");
	query=concatString(query,"'");
	query=concatString(query,email);
	query=concatString(query,"'");
	if(mysql_query(*connection,query) == 0){
		printf("\nOperation Confirmed : credit\n");
		printf("your new balance is : %s\n",newBalance);
	}else{
		printf("%s\n",mysql_error(*connection));
	}
	free(query);
	query=NULL;
}

void clearBuffer(){
	char clear;
	while((clear = getchar()) != '\n' && clear != EOF);
}

char* concatString(char *string1, char *string2){
    char *result = (char*)malloc(strlen(string1) + strlen(string2) + 1);
    strcpy(result, string1);
    strcat(result, string2);
    return result;
}
