     #include <stdio.h>
        #include <stdlib.h>
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/wait.h>
        #include <sys/mman.h>
        #include <fcntl.h>
        #include <string.h>
        #include <sys/ipc.h>
        #include <sys/shm.h>
         
        #define MAX_CUSTOMERS 5
        #define MAX_MENU_ITEMS 9
        #define MAX_ITEM_NAME_LENGTH 50
        #define MAX_SHARED_MEM_SIZE 1024
        //#define shm_id 0x3456
        
         int tot=0;
        // Structure to hold menu items
        struct MenuItem {
            int serialNumber;
            char name[50];
            int price;
        };
         
        // Structure to hold order details
        struct Order {
            int tableNumber;
            int numCustomers;
            int customerOrders[MAX_CUSTOMERS][MAX_MENU_ITEMS];
            int terminate;
            int total;
        };
        
        void calculateBill(struct Order *order, struct MenuItem menu[], int *totalBill) {
            // Function to calculate total bill
            *totalBill = 0;
            for (int i = 0; i < order->numCustomers; ++i) {
                for (int j = 0; j < MAX_MENU_ITEMS; ++j) {
                    if (order->customerOrders[i][j] == -1) {
                        break; // End of order for this customer
                    }
                    int itemIndex = order->customerOrders[i][j] - 1; // Convert from 1-based to 0-based index
                    if (itemIndex < MAX_MENU_ITEMS) {
                        *totalBill += menu[itemIndex].price;
                    }
                }
            }
            //printf("%d\n",tot);
            
        }
         
        void createCustomerProcesses(int numCustomers, int tableNumber, int pipeWriteEnds[][2]) {
            // Function to create customer processes
            for (int i = 0; i < numCustomers; ++i) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("Error forking");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process (customer)
                    close(pipeWriteEnds[i][1]); // Close write end of pipe
                    // Code for customer process can be added here
                    exit(EXIT_SUCCESS);
                } else {
                    // Parent process
                    close(pipeWriteEnds[i][0]); // Close read end of pipe
                }
            }
        }
         
        void readMenu(const char *filename, struct MenuItem menu[], int *numItems) {
            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
         
            // Read menu items from the file
            int i = 0;
            while (fscanf(file, "%d. %49[^0-9] %d INR", &menu[i].serialNumber, menu[i].name, &menu[i].price) == 3) {
                i++;
            }
         
            *numItems = i; // Set the number of menu items
         
            fclose(file);
        }
         
        void displayMenu(struct MenuItem menu[]) {
            // Function to display menu
            for (int i = 0; i < MAX_MENU_ITEMS; ++i) {
                printf("%d. %s %d INR\n", i + 1, menu[i].name, menu[i].price);
            }
        }
         
        void communicateOrders(int numCustomers, int pipeReadEnds[][2], struct Order *order, struct MenuItem menu[]) {
            // Function to communicate orders from customers to table process
            int invalidOrder = 0;
            do {
                invalidOrder = 0;
                for (int i = 0; i < numCustomers; ++i) {
                    close(pipeReadEnds[i][1]); // Close write end of pipe
                    int customerOrders[MAX_MENU_ITEMS] = {0}; // Array to store customer orders
                    int orderIndex = 0;
                    int menuItem;
                    printf("Customer %d: Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", i+1);
                    while (1) {
                        printf("Item %d: ", orderIndex+1);
                        scanf("%d", &menuItem);
                        if (menuItem == -1) {
                            break; // End of order for this customer
                        } else if (menuItem < 1 || menuItem > MAX_MENU_ITEMS) {
                            printf("Invalid menu item number. Please enter a valid menu item number.\n");
                            invalidOrder = 1; // Marking the order as invalid
                            break; // Break inner loop to reorder for all customers
                        }
                        customerOrders[orderIndex++] = menuItem;
                    }
                    if (invalidOrder) {
                        break; // Break outer loop to reorder for all customers
                    }
                    // Copy customer orders to the order struct
                    for (int j = 0; j < MAX_MENU_ITEMS; ++j) {
                        order->customerOrders[i][j] = customerOrders[j];
                        if(customerOrders[j]!=-1)
                        tot+=menu[customerOrders[j]-1].price;
                        //printf("%d\n",menu[customerOrders[j]-1].price);
                    }
                    //order->total=tot;
		 	
                    close(pipeReadEnds[i][0]); // Close read end of pipe
                }
            } while (invalidOrder); // Repeat if any invalid order is placed
        order->total=tot;
        }
         
        void communicateBill(int totalBill) {
            // Function to communicate total bill to table process
            printf("The total bill amount is %d INR.\n", tot);
        }
         
        int main() {
            int tableNumber;
            printf("Enter Table Number: ");
            scanf("%d", &tableNumber);
         
            int numCustomers;
            printf("Enter Number of Customers at Table (maximum no. of customers can be 5): ");
            scanf("%d", &numCustomers);
         
            int numItems;
            const char *filename = "menu.txt";
         
            // Creating pipes for communication between table process and customer processes
            int pipeEnds[MAX_CUSTOMERS][2];
            for (int i = 0; i < numCustomers; ++i) {
                if (pipe(pipeEnds[i]) == -1) {
                    perror("Error creating pipe");
                    exit(EXIT_FAILURE);
                }
            }
            
            key_t shm_id = ftok("shmfile.txt",1);
         
            // Creating shared memory segment
            int shm_fd = shmget(shm_id+tableNumber,MAX_SHARED_MEM_SIZE,IPC_CREAT|0666);
            if (shm_fd == -1) {
                perror("Error creating shared memory");
                exit(EXIT_FAILURE);
            }
            struct Order *order = (struct Order *) shmat(shm_fd,NULL,0);
            if (order == (void *) -1) {
                perror("Error mapping shared memory");
                exit(EXIT_FAILURE);
            }
         
            // Reading menu from file
            struct MenuItem menu[MAX_MENU_ITEMS];
            readMenu(filename,menu,&numItems);
         
            // Displaying menu
            printf("Menu:\n");
            displayMenu(menu);
         
            // Communicating orders
            communicateOrders(numCustomers, pipeEnds, order, menu);
          // printf("%d\n",tot);
            // Calculating bill
            int totalBill = 0;
            calculateBill(order, menu, &totalBill);
         
            // Communicating bill
            communicateBill(totalBill);
         
            // Detaching and removing shared memory
           // shmdt((void *) order);
           // shmctl(shm_fd, IPC_RMID, NULL);
          close(shm_fd);
            shm_unlink("/shared_memory");
         
            // Asking for next set of customers
            int nextSet;
            printf("Enter Number of Customers at Table (maximum no. of customers can be 5) or -1 to exit: ");
            scanf("%d", &nextSet);
         
          if (nextSet == -1) {
                // Informing waiter to terminate
                printf("Notifying waiter process to terminate...\n");
                // Code for notifying waiter to terminate can be added here
                order->terminate=1;
                printf("Table process terminating...\n");
                exit(EXIT_SUCCESS);
            } else {
            order->terminate=0;
            const char*argv[]={"./table",NULL};
                // Resuming from the beginning
                execv(argv[0],(char**)argv); // Assuming the executable file is named "table"
               perror("execv");
               exit(EXIT_FAILURE);
            }
            return 0;
        }
