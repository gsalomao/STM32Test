/**
 * @brief   Assert module to check expressions.
 */

#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * The ASSERT macro is used to check expression. If the expression is false,
 * it calls the function assert_failed reporting the name of the source file
 * and the source line number of the call that failed.
 *
 * @param   expr    Expression to be checked.
 */
#define ASSERT(expr) (expr) ? \
        (void) 0 : ASSERT_Failed((uint8_t *)__FILE__, __LINE__)

void ASSERT_Failed(uint8_t *file, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H */

