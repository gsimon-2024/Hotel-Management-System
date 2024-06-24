     #include <stdio.h>
     
        #include <stdlib.h>
     
        #include <sys/ipc.h>
     
        #include <sys/shm.h>
     
        #include <string.h>
     
        #include <unistd.h>
     
        #define SHM_SIZE 1024  // Define the size of the shared memory segment
     
        //#define shm_id 0x3425
     
        //#define SHM_KEY_MANAGER 0x5678
     
        int prevEarnings=0;
     
        int readEarningsFromSharedMemory(int shmID);
     
        void writeEarningsToFile(int tableNumber, int earnings);
     
        void calculateAndRecordTotals();
     
        int checkNoCustomersLeft(); // Simulated check for no customers
     
        typedef struct {
     
            int totalEarnings;
     
            int closeHotel;
     
        } AdminToManager;
     
        typedef struct 
     
        {
       int prev;
        int total;
     
        }ManagerComm;
     
         
     
        int main() 
     
        {
     
            int totalTables;
     
           int* shm_ptr;
     
           int shmid;
     
            printf("Enter the Total Number of Tables at the Hotel: ");
     
            scanf("%d", &totalTables);
     
         
     
            if(totalTables < 1 || totalTables > 10) {
     
                printf("Invalid number of tables. Please enter a value between 1 and 10.\n");
     
                return 1;
     
            }
     
         
     
            printf("Total number of tables (and waiters) to be managed: %d\n", totalTables);
     
        key_t SHM_KEY_MANAGER = ftok("earnings.txt",1); 
        if (SHM_KEY_MANAGER == -1) {
        perror("ftok");
        return 1;
    } 
        
    	int shmIDManager = shmget(SHM_KEY_MANAGER, sizeof(AdminToManager), 0666| IPC_CREAT);
     
            AdminToManager *adminComm = (AdminToManager *)shmat(shmIDManager, NULL, 0);
     
             int shmID[totalTables+1];
     
             ManagerComm *ptr[totalTables+1];
             
     key_t shm_id = ftok("menu.txt",1);  
     
      	for(int i=1;i<=totalTables;i++)
     
      	{
     
      		if((shmid=shmget(shm_id+i,SHM_SIZE,IPC_CREAT|0666))==-1)
     
      		{
     
      		 	perror("shmget waiter");
     
      		 	exit(1);
     
      		 }
     
      		 shmID[i]=shmid;
     
      	
     
        		 ManagerComm *shm_ptr = (ManagerComm *)shmat(shmid, NULL, 0);
     
     
       	 	if (shm_ptr == (ManagerComm *)(-1))
     
       	 	 {
     
           	 		perror("shmat");
     
          	 		exit(1);
     
        		 } 
     
        		 ptr[i]= shm_ptr;	 
     
        		 shm_ptr->total=0;
        		 shm_ptr->prev=0;
        		 
     
     
     
      	
     
      	
     
      	}
     
      
      	
     
      	while( adminComm->closeHotel!=1)
     
      	{
     
      	   
     
      	}
   	for(int i=1;i<=totalTables;i++)
     
      	{
     
                  writeEarningsToFile(i , ptr[i]->total);
               
     
      	}
     
      	
     
         
     
           
     
            
     
           
     
            	if(checkNoCustomersLeft()) {
     
                		calculateAndRecordTotals();
     
                		printf("Thank you for visiting the Hotel!\n");
     
            	}
     
            
     
            
     
            return 0;
     
        }
     
         
   
     
         
     
        void writeEarningsToFile(int tableNumber,int earnings) {
     
            FILE *fp = fopen("earnings.txt", "a");
     
            if (fp == NULL) {
     
                printf("Error opening file!\n");
     
                return;
     
            }
     
            
     
            fprintf(fp, "Earning from Table %d: %d INR\n", tableNumber, earnings);
     
            
     
            fclose(fp);
     
        }
     
         
     
        void calculateAndRecordTotals() {
     
            FILE *fp = fopen("earnings.txt", "r+");
     
            if (fp == NULL) {
     
                printf("Error opening file!\n");
     
                return;
     
            }
     
            
     
            int totalEarnings = 0, tableEarnings;
     
            char line[100];
     
            
     
            while(fgets(line, sizeof(line), fp)) {
     
                if (sscanf(line, "Earning from Table %*d: %d", &tableEarnings) == 1) {
     
                    totalEarnings += tableEarnings;
     
                }
     
            }
     
         
     
            int totalWages = totalEarnings * 0.4;
     
            int totalProfit = totalEarnings - totalWages;
     
         
     
            fseek(fp, 0, SEEK_END);
     
            fprintf(fp, "\nTotal Earnings of Hotel: %d INR\n", totalEarnings);
     
            fprintf(fp, "Total Wages of Waiters: %d INR\n", totalWages);
     
            fprintf(fp, "Total Profit: %d INR\n", totalProfit);
     
            
     
            printf("\nTotal Earnings of Hotel: %d INR\n", totalEarnings);
     
            printf("Total Wages of Waiters: %d INR\n", totalWages);
     
            printf("Total Profit: %d INR\n", totalProfit);
     
            
     
            fclose(fp);
     
        }
     
         
     
        int checkNoCustomersLeft() {
     
            // Simulated logic for checking if there are no customers left
     
            // In a real scenario, this would involve checking shared resources or signals
     
            
     
            // Assuming all customers have left
     
            
     
            
     
            return 1;
     
        }
