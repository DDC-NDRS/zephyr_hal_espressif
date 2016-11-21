#ifndef __PCNT_H__
#define __PCNT_H__

#include <esp_types.h>
#include "esp_intr.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "soc/soc.h"
#include "soc/pcnt_reg.h"
#include "soc/pcnt_struct.h"
#include "soc/gpio_sig_map.h"
#include "driver/gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

#define PCNT_PIN_NOT_USED  (-1)

typedef enum {
    PCNT_MODE_KEEP = 0,             /*!< Control mode: won't change counter mode*/
    PCNT_MODE_REVERSE = 1,          /*!< Control mode: invert counter mode(increase -> decrease, decrease -> increase);*/
    PCNT_MODE_DISABLE = 2,          /*!< Control mode: Inhibit counter(counter value will not change in this condition)*/
    PCNT_MODE_MAX
} pcnt_ctrl_mode_t;

typedef enum {
    PCNT_COUNT_DIS = 0,            /*!< Counter mode: Decrease counter value*/
    PCNT_COUNT_INC = 1,            /*!< Counter mode: Increase counter value*/
    PCNT_COUNT_DEC = 2,            /*!< Counter mode: Inhibit counter(counter value will not change in this condition)*/
    PCNT_COUNT_MAX
} pcnt_count_mode_t;

typedef enum {
    PCNT_UNIT0 = 0,                 /*!< PCNT unit0 */
    PCNT_UNIT1 = 1,                 /*!< PCNT unit1 */
    PCNT_UNIT2 = 2,                 /*!< PCNT unit2 */
    PCNT_UNIT3 = 3,                 /*!< PCNT unit3 */
    PCNT_UNIT4 = 4,                 /*!< PCNT unit4 */
    PCNT_UNIT5 = 5,                 /*!< PCNT unit5 */
    PCNT_UNIT6 = 6,                 /*!< PCNT unit6 */
    PCNT_UNIT7 = 7,                 /*!< PCNT unit7 */
    PCNT_UNIT_MAX,
} pcnt_unit_t; 

typedef enum{
    PCNT_CHANNEL_0 = 0x00,           /*!< PCNT channel0 */
    PCNT_CHANNEL_1 = 0x01,           /*!< PCNT channel1 */
    PCNT_CHANNEL_MAX,
} pcnt_channel_t;

typedef enum {
    PCNT_EVT_L_LIM = 0,             /*!< PCNT watch point event: Minimum counter value */
    PCNT_EVT_H_LIM = 1,             /*!< PCNT watch point event: Maximum counter value*/
    PCNT_EVT_THRES0 = 2,            /*!< PCNT watch point event: threshold0 value event*/
    PCNT_EVT_THRES1 = 3,            /*!< PCNT watch point event: threshold1 value event*/
    PCNT_EVT_ZERO = 4,              /*!< PCNT watch point event: counter value zero event*/
    PCNT_EVT_MAX
} pcnt_evt_type_t;

/**
 * @brief PCNT configure struct
 */
typedef struct {
    int pulse_gpio_num;             /*!< Pulse input gpio_num, if you want to use gpio16, pulse_gpio_num = 16, a negative value will be ignored */
    int ctrl_gpio_num;              /*!< Contol signal input gpio_num, a negative value will be ignored*/
    pcnt_ctrl_mode_t lctrl_mode;    /*!< PCNT low control mode*/
    pcnt_ctrl_mode_t hctrl_mode;    /*!< PCNT high control mode*/
    pcnt_count_mode_t pos_mode;     /*!< PCNT positive edge count mode*/
    pcnt_count_mode_t neg_mode;     /*!< PCNT negative edge count mode*/
    int16_t counter_h_lim;          /*!< Maximum counter value */
    int16_t counter_l_lim;          /*!< Minimum counter value */
    pcnt_unit_t unit;               /*!< PCNT unit number */
    pcnt_channel_t channel;         /*!< the PCNT channel */
} pcnt_config_t;

/**
 * @brief Configure PCNT unit
 *
 * @param pcnt_config Pointer of PCNT unit configure parameter
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_unit_config(pcnt_config_t *pcnt_config);

/**
 * @brief Get pulse counter value
 *
 * @param pcnt_unit  PCNT unit number
 * @param count Pointer to accept counter value
 *
 * @return  
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_get_counter_value(pcnt_unit_t pcnt_unit, int16_t* count);

/**
 * @brief Pause PCNT counter of PCNT unit
 *
 * @param pcnt_unit PCNT unit number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_counter_pause(pcnt_unit_t pcnt_unit);

/**
 * @brief Resume counting for PCNT counter
 *
 * @param pcnt_unit PCNT unit number, select from pcnt_unit_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_counter_resume(pcnt_unit_t pcnt_unit);

/**
 * @brief Clear and reset PCNT counter value to zero
 *
 * @param  pcnt_unit PCNT unit number, select from pcnt_unit_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_counter_clear(pcnt_unit_t pcnt_unit);

/**
 * @brief Enable PCNT interrupt for PCNT unit
 *        @note
 *        Five watch point events share the same interrupt source for each unit.
 *
 * @param pcnt_unit PCNT unit number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_intr_enable(pcnt_unit_t pcnt_unit);

/**
 * @brief Disable PCNT interrupt for PCNT uint
 *
 * @param pcnt_unit PCNT unit number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_intr_disable(pcnt_unit_t pcnt_unit);

/**
 * @brief Enable PCNT event of PCNT unit
 *
 * @param unit PCNT unit number
 * @param evt_type PCNT watch point event type, five events share a same interrupt source
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_event_enable(pcnt_unit_t unit, pcnt_evt_type_t evt_type);

/**
 * @brief Disable PCNT event of PCNT unit
 *
 * @param unit PCNT unit number
 * @param evt_type PCNT watch point event type, five events share a same interrupt source
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_event_disable(pcnt_unit_t unit, pcnt_evt_type_t evt_type);

/**
 * @brief Set PCNT event value of PCNT unit
 *
 * @param unit PCNT unit number
 * @param evt_type PCNT watch point event type, five events share a same interrupt source
 * @param value Counter value for PCNT event
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_set_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t value);

/**
 * @brief Get PCNT event value of PCNT unit
 *
 * @param unit PCNT unit number
 * @param evt_type PCNT watch point event type, five events share a same interrupt source
 * @param value Pointer to accept counter value for PCNT event
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_get_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t *value);

/**
 * @brief Register PCNT interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *        @note
 *        Users should know that which CPU is running and then pick a INUM that is not used by system.
 *        We can find the information of INUM and interrupt level in soc.h.
 *
 * @param pcnt_intr_num PCNT interrupt number, check the info in soc.h, and please see the core-isa.h for more details
 * @param fn Interrupt handler function.
 *        @note
 *        Note that the handler function MUST be defined with attribution of "IRAM_ATTR".
 * @param arg Parameter for handler function
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t pcnt_isr_register(uint32_t pcnt_intr_num, void (*fn)(void*), void * arg);

/**
 * @brief Configure PCNT pulse signal input pin and control input pin
 *
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param pulse_io Pulse signal input GPIO
 * @param ctrl_io Control signal input GPIO
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_set_pin(pcnt_unit_t unit, pcnt_channel_t channel, int pulse_io, int ctrl_io);

/**
 * @brief Enable PCNT input filter
 *
 * @param unit PCNT unit number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_filter_enable(pcnt_unit_t unit);

/**
 * @brief Disable PCNT input filter
 *
 * @param unit PCNT unit number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_filter_disable(pcnt_unit_t unit);

/**
 * @brief Set PCNT filter value
 *
 * @param unit PCNT unit number
 * @param filter_val PCNT signal filter value, counter in APB_CLK cycles.
 *        Any pulses lasting shorter than this will be ignored when the filter is enabled.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_set_filter_value(pcnt_unit_t unit, uint16_t filter_val);

/**
 * @brief Get PCNT filter value
 *
 * @param unit PCNT unit number
 * @param filter_val Pointer to accept PCNT filter value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_get_filter_value(pcnt_unit_t unit, uint16_t *filter_val);

/**
 * @brief Set PCNT counter mode
 *
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param pos_mode Counter mode when detecting positive edge
 * @param neg_mode Counter mode when detecting negative edge
 * @param hctrl_mode Counter mode when control signal is high level
 * @param lctrl_mode Counter mode when control signal is low level
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pcnt_set_mode(pcnt_unit_t unit, pcnt_channel_t channel, pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode);


/***************************EXAMPLE**********************************
 *
 *
 * ----------------EXAMPLE OF LEDC SETTING ---------------------
 * @code{c}
 * //1. Config PCNT unit
 * pcnt_config_t pcnt_config = {
 *     .pulse_gpio_num = 4,         //set gpio4 as pulse input gpio
 *     .ctrl_gpio_num = 5,          //set gpio5 as control gpio
 *     .channel = PCNT_CHANNEL_0,         //use unit 0 channel 0
 *     .lctrl_mode = PCNT_MODE_REVERSE,   //when control signal is low ,reverse the primary counter mode(inc->dec/dec->inc)
 *     .hctrl_mode = PCNT_MODE_KEEP,      //when control signal is high,keep the primary counter mode
 *     .pos_mode = PCNT_COUNT_INC,        //increment the counter
 *     .neg_mode = PCNT_COUNT_DIS,        //keep the counter value
 *     .counter_h_lim = 10,
 *     .counter_l_lim = -10,
 * };
 * pcnt_unit_config(&pcnt_config);        //init unit
 * @endcode
 *
 * @code{c}
 * //2. Configure PCNT watchpoint event.
 * pcnt_set_event_value(PCNT_UNIT0, PCNT_EVT_THRES1, 5);   //set thres1 value
 * pcnt_event_enable(PCNT_UNIT0, PCNT_EVT_THRES1);         //enable thres1 event
 * @endcode
 *
 * For more examples please refer to PCNT example code in IDF_PATH/examples
 *
 *--------------------------END OF EXAMPLE --------------------------
 */

#ifdef __cplusplus 
}
#endif


#endif
