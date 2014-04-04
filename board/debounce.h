//-----------------------------------------------------------------------------
/*

Switch Input Debouncing

*/
//-----------------------------------------------------------------------------

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

//-----------------------------------------------------------------------------

#define DEBOUNCE_COUNT 4

typedef struct debounce_ctrl {

    uint32_t sample[DEBOUNCE_COUNT];
    uint32_t state;
    int idx;
    int monitor;
    void (*action_on)(uint32_t val);
    void (*action_off)(uint32_t val);

} DEBOUNCE_CTRL;

extern DEBOUNCE_CTRL debounce;

//-----------------------------------------------------------------------------
// api

void debounce_init(void);
void debounce_isr(void);
uint32_t debounce_rd(void);

//-----------------------------------------------------------------------------

#endif // DEBOUNCE_H

//-----------------------------------------------------------------------------
