#ifndef AWS_H
#define AWS_H
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* POSIX includes. */
#include <unistd.h>
#include <inttypes.h>

/* shadow demo helpers header. */
#include "shadow_demo_helpers.h"

/* Shadow config include. */
#include "shadow_config.h"

/* SHADOW API header. */
#include "shadow.h"

/* JSON API header. */
#include "core_json.h"

/* Clock for timer. */
#include "clock.h"

/**
 * @brief Format string representing a Shadow document with a "desired" state.
 *
 * The real json document will look like this:
 * {
 *   "state": {
 *     "desired": {
 *       "powerOn": 1
 *     }
 *   },
 *   "clientToken": "021909"
 * }
 *
 * Note the client token, which is optional for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_DESIRED_JSON     \
    "{"                         \
    "\"state\":{"               \
    "\"reported\":{"            \
    "\"temperature\":%2f,"      \
    "\"humidity\":%2f,"         \
    "\"pressure\":%2f,"         \
    "\"gas\":%2f"               \
    "},"                        \
    "}"

/**
 * @brief The expected size of #SHADOW_DESIRED_JSON.
 *
 * Because all the format specifiers in #SHADOW_DESIRED_JSON include a length,
 * its full actual size is known by pre-calculation, here's the formula why
 * the length need to minus 3:
 * 1. The length of "%01d" is 4.
 * 2. The length of %06lu is 5.
 * 3. The actual length we will use in case 1. is 1 ( for the state of powerOn ).
 * 4. The actual length we will use in case 2. is 6 ( for the clientToken length ).
 * 5. Thus the additional size 3 = 4 + 5 - 1 - 6 + 1 (termination character).
 *
 * In your own application, you could calculate the size of the json doc in this way.
 */
#define SHADOW_DESIRED_JSON_LENGTH    ( sizeof( SHADOW_DESIRED_JSON ) + 20 )

/**
 * @brief Format string representing a Shadow document with a "reported" state.
 *
 * The real json document will look like this:
 * {
 *   "state": {
 *     "reported": {
 *       "powerOn": 1
 *     }
 *   },
 *   "clientToken": "021909"
 * }
 *
 * Note the client token, which is required for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_REPORTED_JSON    \
    "{"                         \
    "\"state\":{"               \
    "\"reported\":{"            \
    "\"temperature\":%2f,"      \
    "\"humidity\":%2f,"         \
    "\"pressure\":%2f,"         \
    "\"gas\":%2f"               \
    "},"                        \
    "}"

#define SHADOW_REPORTED_JSON_LENGTH    ( sizeof( SHADOW_REPORTED_JSON ) + 20 )

/**
 * @brief The maximum number of times to run the loop in this demo.
 *
 * @note The demo loop is attempted to re-run only if it fails in an iteration.
 * Once the demo loop succeeds in an iteration, the demo exits successfully.
 */
#ifndef SHADOW_MAX_DEMO_LOOP_COUNT
    #define SHADOW_MAX_DEMO_LOOP_COUNT    ( 3 )
#endif

/**
 * @brief Time in seconds to wait between retries of the demo loop if
 * demo loop fails.
 */
#define DELAY_BETWEEN_DEMO_RETRY_ITERATIONS_S           ( 5 )

/**
 * @brief JSON key for response code that indicates the type of error in
 * the error document received on topic `/delete/rejected`.
 */
#define SHADOW_DELETE_REJECTED_ERROR_CODE_KEY           "code"

/**
 * @brief Length of #SHADOW_DELETE_REJECTED_ERROR_CODE_KEY
 */
#define SHADOW_DELETE_REJECTED_ERROR_CODE_KEY_LENGTH    ( ( uint16_t ) ( sizeof( SHADOW_DELETE_REJECTED_ERROR_CODE_KEY ) - 1 ) )



// Function declarations for AWS IoT
void aws_task(void *pvParameter);


#endif // AWS_H