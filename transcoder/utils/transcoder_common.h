#ifndef TRANSCODER_COMMON_H
#define TRANSCODER_COMMON_H

#include <QString>
/*! @brief Error status */

typedef enum TranscoderError
{
    TSR_SUCCESS,         /**< @brief Success, no errors */
    TSR_DECODER_START,   /**< @brief Invalid mezzanine context */
    TSR_DECODER_END,     /**< @brief Invalid frame number */
    TSR_DECODER_ERROR,   /**< @brief Invalid metadata */

    TSR_ENCODER_START,  /**< @brief Invalid mezzanine context */
    TSR_ENCODER_END,    /**< @brief Invalid frame number */
    TSR_ENCODER_ERROR,  /**< @brief Invalid metadata */


    TSR_WRITER_START,   /**< @brief Invalid mezzanine context */
    TSR_WRITER_END,     /**< @brief Invalid frame number */
    TSR_WRITER_ERROR,   /**< @brief Invalid metadata */

    TSR_WRITER_OPEN_ERROR,
    TSR_WRITER_CLOSE_ERROR,

    TSR_USER_CANCEL
} TranscoderError;

/*! @brief Returns a human-readable error message.
 * The string returned should not be freed.
 * @param e Error code
 * @returns NULL is the error code is unknown, otherwise a message string.
 */
QString getTranscoderErrorStr(TranscoderError e);

#endif // TRANSCODER_COMMON_H
