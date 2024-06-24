    #include <stdio.h>
     
    #include <stdlib.h>
     
    #include <unistd.h>
     
    #include <sys/ipc.h>
     
    #include <sys/shm.h>
     
    #include <sys/types.h>
     
     
     
    #define MAX_TABLES 10
    #define MAX_CUSTOMERS 5
    #define MAX_MENU_ITEMS 9
    #define MAX_ITEMS 4
    #define max 1024
    //#define SHM_KEY_BASE 0x4567
    //#define shm_key 0x3425
    // #define shm_id 0x3456
     
    typedef struct Order {
            int tableNumber;
            int numCustomers;
            int customerOrders[MAX_CUSTOMERS][MAX_MENU_ITEMS];
            int terminate;
            int total;
           
        }TableOrder;
     
     
     
    typedef struct {
     
        //int earningsFromTable[MAX_TABLES];
        int tot;
        int prev;
     
    } ManagerComm;
     
     
     
    typedef struct {
     
        char itemName[50];
     
        int itemPrice;
     
    } MenuItem;
     
     
     
    /*int isValidOrder(int order[], int itemCount) {
     
        for (int i = 0; i < itemCount; ++i) {
     
            if (order[i] < 1 || order[i] > MAX_ITEMS) {
     
                return 0; // Invalid item number
     
            }
     
        }
     
        return 1; // Valid order
     
    }*/
     
     
     
    /*int calculateTotalBill(MenuItem menu[], int order[], int itemCount) {
     
        int total = 0;
     
        for (int i = 0; i < itemCount; ++i) {
     
            total += menu[order[i] - 1].itemPrice;
     
        }
     
        return total;
     
    }*/
     
     
     
    int main() {
     
        int waiterID;
     
        printf("Enter Waiter ID: ");
     
        scanf("%d", &waiterID);
     
     
     
        if (waiterID < 1 || waiterID > MAX_TABLES) {
     
            fprintf(stderr, "Invalid Waiter ID. It should be between 1 and %d.\n", MAX_TABLES);
     
            return 1;
     
        }
        
        key_t shm_id = ftok("shmfile.txt",1);
     key_t shm_key = ftok("menu.txt",1);     
     
     
        key_t shmKeyTable = shm_id + waiterID;
     
        key_t shmKeyManager = shm_key + waiterID;
     
     
     
        int shmIDTable = shmget(shmKeyTable, max, 0666 | IPC_CREAT);
     
        if (shmIDTable < 0) {
     
            perror("shmget table");
     
            return 1;
     
        }
     
     
     
        TableOrder *tableOrder = (TableOrder *)shmat(shmIDTable, NULL, 0);
     
        if (tableOrder == (void *)-1) {
     
            perror("shmat table");
     
            return 1;
     
        }
     
     
     
        int shmIDManager = shmget(shmKeyManager, sizeof(ManagerComm), 0666 | IPC_CREAT);
     
        if (shmIDManager < 0) {
     
            perror("shmget manager");
     
            return 1;
     
        }
     
     
     
        ManagerComm *managerComm = (ManagerComm *)shmat(shmIDManager, NULL, 0);
     
        if (managerComm == (void *)-1) {
     
            perror("shmat manager");
     
            return 1;
     
        }
     
     
     
        // Read menu from file
     
        FILE *menuFile = fopen("menu.txt", "r");
     
        if (!menuFile) {
     
            perror("Error opening menu file");
     
            return 1;
     
        }
     
     
     
        MenuItem menu[MAX_ITEMS];
     
        for (int i = 0; i < MAX_ITEMS; ++i) {
     
            if (fscanf(menuFile, "%*d. %49[^0-9]%d INR", menu[i].itemName, &menu[i].itemPrice) != 2) {
     
                fprintf(stderr, "Error reading menu file\n");
     
                fclose(menuFile);
     
                return 1;
     
            }
     
        }
     
     
     
        fclose(menuFile);
     
     
     
        // Waiter process main loop
        managerComm->tot=0;
     managerComm->prev=0;
  
     
    while (!tableOrder->terminate) {
     
        
        int tableNumber = waiterID;  // Assuming waiterID corresponds to table number
     
       
        tableOrder->terminate=0;
        
       
     
       
       printf("Waiter %d: Bill Amount for Table %d: %d INR\n", waiterID, tableNumber, tableOrder->total);
       managerComm->prev=managerComm->tot;
       managerComm->tot+=tableOrder->total;
 	tableOrder->total=0;
         
if( tableOrder->numCustomers==-1)
	tableOrder->terminate=1;
	



     
       
        sleep(1);
    }
             tableOrder->terminate=0;
    // Detach from shared memory
    shmdt(tableOrder);
    shmdt(managerComm);
     
    printf("Waiter %d terminating...\n", waiterID);
     
    return 0;
     
     
    }
