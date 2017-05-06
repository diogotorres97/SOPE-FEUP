olha que o semaforo parece uma boa cena
a cada request aceite é só dar sem wait, usleep, sem post 🙂

typedef struct {
        int capacity;
        int number_of_requests; //TODO: Initialize

        int requests_received_fd;
        int requests_rejected_fd;
        int statistics_fd;

        struct timespec starting_time;

        sem_t* sauna_semaphore;
        pthread_mutex_t sauna_mutex;
}sauna_info;

int create_fifos(){
        mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; //Read and write for file owner and group owner

        if(mkfifo("/tmp/entrada", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        return ERROR;
                }
        }

        if(mkfifo("/tmp/rejeitados", permissions) != OK) {
                if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
                        printf("Sauna: %s\n", strerror(errno));
                        return ERROR;
                }
        }

        return OK;
}

int open_fifos(){
        if((general_info.requests_received_fd = open("/tmp/entrada", O_RDONLY)) == ERROR) {
                printf("Sauna: %s\n", strerror(errno));
                return ERROR;
        }

        while((general_info.requests_rejected_fd = open("/tmp/rejeitados", O_WRONLY | O_SYNC)) == ERROR)
                sleep(1);

        return OK;
}
