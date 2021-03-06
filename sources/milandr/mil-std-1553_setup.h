// ID: SPO-UOS-milandr-mil-std-1553_setup.h VER: 1.0.0
//
// История изменений:
//
// 1.0.0	Начальная версия
//

#ifndef __MIL1553_MILANDR_SETUP_H__
#define __MIL1553_MILANDR_SETUP_H__

#include <milandr/mil1553-interface.h>
#include <mem/mem-queue.h>
#include <runtime/lib.h>

#define STATUS_ITEMS_SIZE	32

#define RT_DEBUG               0
#define BC_DEBUG               0

#define MIL_STD_CLOCK_DIV      2

#define LEFT_LED	4
#define RIGHT_LED	2
#define TOP_LED		1

#define TOGLE(n)	((ARM_GPIOB->DATA & (n))?(ARM_GPIOB->DATA &= ~(n)):(ARM_GPIOB->DATA |= (n)))
//#define MIL_STD_SUBADDR_WORD_INDEX(n)	(n*MIL_SUBADDR_WORDS_COUNT)

#define MIL_SUBADDR_COUNT	32

struct mil_txbuf_t
{
	uint32_t            data[MIL_SUBADDR_WORDS_COUNT];
	int					nb_words;
	volatile int		busy;
};
typedef struct mil_txbuf_t mil_txbuf_t;

// Реализация не потоко-защищённая

struct milandr_mil1553_t
{
    mil1553if_t         milif;

    unsigned            mode;
    unsigned            addr_self; // только для RT
    MIL_STD_1553B_t     *reg;
    int                 irq;
    mutex_t             tim_lock;
    TIMER_t             *tim_reg;
    int                 tim_irq;
    mem_pool_t          *pool;
    mem_queue_t         cyclogram_rxq;
    mil_slot_t          *cyclogram;
    mil_slot_t          *cur_slot;
    unsigned            nb_slots;
    unsigned            period_ms;
    mil_slot_desc_t     urgent_desc;
    uint16_t            urgent_data[MIL_SUBADDR_WORDS_COUNT];
    int                 is_running;
    
    // mem_queue_t         rt_rxq;			// только для RT
    mil_txbuf_t			txbuf[MIL_SUBADDR_COUNT]; // только для RT
    mil_txbuf_t			rxbuf[MIL_SUBADDR_COUNT]; // только для RT

    mem_queue_t         urgent_rxq;         // только для BC
    // Статистика
    unsigned            nb_words;
    unsigned            nb_commands;
    unsigned            nb_lost;
    unsigned            nb_errors;
    unsigned            nb_emergency_errors;
    unsigned            queue_len;
    unsigned            nb_transmits;
};

typedef struct milandr_mil1553_t milandr_mil1553_t;

struct status_item_t {
    volatile uint32_t status;
    volatile uint16_t command_word_1;
    volatile uint16_t msg;
    volatile uint32_t time_stamp;
    volatile uint32_t done;
    volatile uint32_t error;
    volatile uint32_t status_word_1;
};

typedef struct status_item_t status_item_t;

extern status_item_t status_array[STATUS_ITEMS_SIZE];
extern int read_idx;

// nb_rxq_msg и timer могут быть равными 0. nb_rxq_msg == 0 означает, что не нужно использовать приёмную очередь.
// Если nb_rxq_msg, то параметр pool не используется.
// timer может быть равен 0, если драйвер используется только в режиме ОУ, либо если в режиме КШ циклограмма должна
// всегда выполняться с минимальным периодом.
void milandr_mil1553_init(milandr_mil1553_t *mil, int port, mem_pool_t *pool, unsigned nb_rxq_msg, TIMER_t *timer);

void milandr_mil1553_init_pins(int port);
void mil_rt_send(mil1553if_t *_mil, int subaddr, void *data, int nb_words);
void mil_rt_receive(mil1553if_t *_mil, int subaddr, void *data, int *nb_words);
void mil_rt_send_16(mil1553if_t *_mil, int subaddr, void *data, int nb_words);
void mil_rt_receive_16(mil1553if_t *_mil, int subaddr, void *data, int *nb_words);

#endif

