#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "ringBuffer.h"
#include "serial.h"
#include "yrosserial.h"

void *threadReadDataLoop(void *args)
{
    while (1)
    {
        yRosSerial_spin();
        sleep(5);
    }

    return NULL;
}

int main()
{
    pthread_t threadParse;

    pthread_create(&threadParse, NULL, threadReadDataLoop, NULL);

    yRosSerial_setting_t s;
    s.rxBufSize = 256;
    s.txBufSize = 256;
    yRosSerial_init(&s);
    while (1)
    {
        yRosSerial_receive();

        usleep(1000);
    }
    return 0;
}