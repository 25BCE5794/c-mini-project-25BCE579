/*
Project Title: Mini Banking System with Transaction Log
Student Name: 
Register No: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Account {
    int accNo;
    char name[50];
    float balance;
};

/* Function Prototypes */
void createAccount();
void deposit();
void withdraw();
void viewAccount();
void searchAccount();
void showLastTransactions(int accNo);
void logTransaction(int accNo, char type[], float amount);
int accountExists(int accNo);

int main() {

    int choice;

    while(1) {

        printf("\n====== MY DIGITAL BANK SYSTEM ======\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. View All Accounts\n");
        printf("5. Search Account\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d",&choice);

        switch(choice) {

            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: viewAccount(); break;
            case 5: searchAccount(); break;
            case 6: exit(0);

            default:
            printf("Invalid choice!\n");
        }
    }
}

/* Create Account */

void createAccount() {

    struct Account a;

    printf("Enter Account Number: ");
    scanf("%d",&a.accNo);

    if(a.accNo <= 0) {
    printf("Invalid Account Number!\n");
    return;
    }

    if(accountExists(a.accNo)) {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s",a.name);

    printf("Enter Initial Balance: ");
    scanf("%f",&a.balance);

    if(a.balance < 0) {
        printf("Balance cannot be negative!\n");
        return;
    }

    FILE *fp = fopen("bankdata.dat","ab");
    if(fp == NULL) {
    printf("File error!\n");
    return;
    }
    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);

    printf("Account created successfully!\n");
}

/* Deposit */

void deposit() {

    int acc;
    float amount;
    struct Account a;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    printf("Enter Amount to Deposit: ");
    scanf("%f",&amount);

    if(amount <= 0) {
        printf("Invalid amount!\n");
        return;
    }

    FILE *fp = fopen("bankdata.dat","rb+");

    while(fread(&a,sizeof(a),1,fp)) {

        if(a.accNo == acc) {

            a.balance += amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            fclose(fp);

            logTransaction(acc,"DEPOSIT",amount);

            printf("Deposit successful!\n");
            return;
        }
    }

    printf("Account not found!\n");
    fclose(fp);
}

/* Withdraw */

void withdraw() {

    int acc;
    float amount;
    struct Account a;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    printf("Enter Amount to Withdraw: ");
    scanf("%f",&amount);

    FILE *fp = fopen("bankdata.dat","rb+");

    while(fread(&a,sizeof(a),1,fp)) {

        if(a.accNo == acc) {

            if(a.balance < amount) {
                printf("Insufficient Balance!\n");
                fclose(fp);
                return;
            }

            a.balance -= amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            fclose(fp);

            logTransaction(acc,"WITHDRAW",amount);

            printf("Withdrawal successful!\n");
            return;
        }
    }

    printf("Account not found!\n");
    fclose(fp);
}

/* View All Accounts */

void viewAccount() {

    struct Account a;

    FILE *fp = fopen("bankdata.dat","rb");

    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    printf("\nAccNo\tName\tBalance\n");

    while(fread(&a,sizeof(a),1,fp)) {
        printf("%d\t%s\t%.2f\n",a.accNo,a.name,a.balance);
    }

    fclose(fp);
}

/* Search Account */

void searchAccount() {

    int acc;
    struct Account a;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    FILE *fp = fopen("bankdata.dat","rb");

    while(fread(&a,sizeof(a),1,fp)) {

        if(a.accNo == acc) {

            printf("\nAccount Found\n");
            printf("AccNo: %d\n",a.accNo);
            printf("Name: %s\n",a.name);
            printf("Balance: %.2f\n",a.balance);

            showLastTransactions(acc);

            fclose(fp);
            return;
        }
    }

    printf("Account not found!\n");
    fclose(fp);
}

/* Check if account exists */

int accountExists(int accNo) {

    struct Account a;

    FILE *fp = fopen("bankdata.dat","rb");

    if(fp == NULL)
        return 0;

    while(fread(&a,sizeof(a),1,fp)) {
        if(a.accNo == accNo) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Log Transaction */

void logTransaction(int accNo, char type[], float amount) {

    FILE *fp = fopen("translog.txt","a");

    time_t t;
    time(&t);

    fprintf(fp,"%d,%s,%.2f,%s",accNo,type,amount,ctime(&t));

    fclose(fp);
}

/* Show Last 5 Transactions */

void showLastTransactions(int accNo) {

    FILE *fp = fopen("translog.txt","r");

    if(fp == NULL) {
        printf("No transactions found.\n");
        return;
    }

    char line[200];
    char buffer[100][200];
    int count = 0;

    while(fgets(line,sizeof(line),fp)) {

        int acc;
        sscanf(line,"%d",&acc);

        if(acc == accNo) {
            strcpy(buffer[count++],line);
        }
    }

    printf("\nLast Transactions:\n");

    int start = count - 5;
    if(start < 0) start = 0;

    for(int i=start;i<count;i++) {
        printf("%s",buffer[i]);
    }

    fclose(fp);
}