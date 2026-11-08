#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// stack size 4 producer thread & cons
#define PRODUCER_STACK_SIZE 512 
#define CONSUMER_STACK_SIZE 512
// priorities 4 producer & cons thread
#define PRODUCER_PRIORITY 5 
#define CONSUMER_PRIORITY 5
// max msgs in queue where queue -> 2 pass data 4m prod 2 cons
#define MSGQ_MAX_MSGS 8

typedef struct {
        uint32_t seq;
        int32_t value;
} sample_msg_t;

// define a sample_msgq message queue w/ max msgs & alignment
K_MSGQ_DEFINE(sample_msgq, sizeof(sample_msg_t), MSGQ_MAX_MSGS, 4);

// @brief prod main thread func...
static void producer_main(void *a, void *b, void *c) {
        ARG_UNUSED(a);
        ARG_UNUSED(b);
        ARG_UNUSED(c);
        
        uint32_t seq = 0;

        for (;;) {
                const sample_msg_t msg = {
                        .seq = seq,
                        .value = (int32_t)(seq * 3) // value is 3x seq 4 testing type shit...
                };

                // put msg in queue, if full drop msg & print seq
                if (k_msgq_put(&sample_msgq, &msg, K_NO_WAIT) != 0) {
                        printk("queue full in prod, dropping seq=%u\n", seq);
                }

                seq++;
                k_msleep(100);
        }
}

// @brief 
static void consumer_main(void *a, void *b, void *c) {
        ARG_UNUSED(a);
        ARG_UNUSED(b);
        ARG_UNUSED(c);

        sample_msg_t msg;

        for(;;) {
                k_msgq_get(&sample_msgq, &msg, K_FOREVER); // block until msg avaiable
                printk("consumer got msg: seq:%u, val:%d\n", msg.seq, msg.value);
        }
}

// define & start producer & consum threads w/ stack size, priority, & entry func
K_THREAD_DEFINE(producer_tid, PRODUCER_STACK_SIZE, producer_main, 
                NULL, NULL, NULL, PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_tid, CONSUMER_STACK_SIZE, consumer_main, 
                NULL, NULL, NULL, CONSUMER_PRIORITY, 0, 0);

int main(void) {
        return 0; // main thread does nothing we have everything goin on already type shit...
}