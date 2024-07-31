#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define QUEUE_KEY 1234
#define ARRAY_SIZE 5
#define MSG_SIZE (ARRAY_SIZE * sizeof(int))
struct msg_buffer {
    long msg_type;
    int msg_text[ARRAY_SIZE];
};
void sender(int msgid) {
    struct msg_buffer message;
    message.msg_type = 1;
    int numbers[] = {1, 2, 3, 4, 5};
    memcpy(message.msg_text, numbers, MSG_SIZE);
    if (msgsnd(msgid, &message, MSG_SIZE, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Child: Message sent: ");
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        printf("%d ", message.msg_text[i]);
    }
    printf("\n");
}
void receiver(int msgid) {
    struct msg_buffer message;

    if (msgrcv(msgid, &message, MSG_SIZE, 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Parent: Message received: ");
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        printf("%d ", message.msg_text[i]);
    }
    printf("\n");

    int sum = 0;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sum += message.msg_text[i];
    }
    printf("Parent: Sum of array: %d\n", sum);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
}
int main() {
    int msgid;
    pid_t pid;
    msgid = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        sender(msgid);
    } else {
        wait(NULL);
        receiver(msgid);
    }

    return 0;
}

