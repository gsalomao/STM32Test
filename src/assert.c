/**
 * @brief   Assert module to check expressions.
 */

#include "assert.h"

/**
 * This function is called when the check (assert) failed. It reports the
 * source file name and source file number which the check failed.
 *
 * @param   file    File name which the check failed.
 * @param   line    Line number which the check failed.
 */
void ASSERT_Failed(uint8_t *file, uint32_t line)
{
    while (1)
        ;
}
