        #include <stdio.h>
     
        #include <stdlib.h>
     
        #include <unistd.h>
     
        #include <sys/ipc.h>
     
        #include <sys/shm.h>
     
        #include <sys/types.h>
     
         
     
       // #define SHM_KEY_MANAGER 0x5678
     
         
     
        typedef struct {
     
            int totalEarnings;
     
            int closeHotel;
     
        } AdminToManager;
     
         
     
        int main() {
     
            char input;
     
            
     
            // Attach to shared memory with hotel manager
     
            //int shmIDManager = shmget(SHM_KEY_MANAGER, sizeof(AdminToManager), 0666);
            
            key_t SHM_KEY_MANAGER = ftok("earnings.txt",1);
            if (SHM_KEY_MANAGER == -1) {
        	perror("ftok");
        	return 1;
    	}
     
            int shmIDManager = shmget(SHM_KEY_MANAGER, sizeof(AdminToManager), 0666| IPC_CREAT);
     
            if (shmIDManager < 0) {
     
                perror("shmget");
     
                exit(1);
     
            }
     
            AdminToManager *adminComm = (AdminToManager *)shmat(shmIDManager, NULL, 0);
     
            if (adminComm == (void *) -1) {
     
                perror("shmat");
     
                exit(1);
     
            }
     
           adminComm->closeHotel = 0;
     
         
     
            // Admin process loop
     
            do {
     
                printf("Do you want to close the hotel? Enter Y for Yes and N for No.\n");
     
                scanf(" %c", &input);
     
            } while (input != 'Y' && input != 'y');
     
         
     
            // Signal to hotel manager to close hotel
     
            adminComm->closeHotel = 1;
     
         
     
            // Detach from shared memory
     
            shmdt(adminComm);
     
         
     
            printf("Admin process terminating...\n");
     
            return 0;
     
        }
