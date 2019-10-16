#ifndef TRANSCODER_COMMON_H
#define TRANSCODER_COMMON_H

#include <string>
#define  DecodedFramesBufferSize 64
/*! @brief Error status */

typedef enum TranscoderError
{
    TSR_SUCCESS,         /**< @brief Success, no errors */
    TSR_DECODER_START,   /**< @brief status for decoder start */
    TSR_DECODER_END,     /**< @brief status for decoder end */
    TSR_DECODER_ERROR,   /**< @brief decoder error, will report to transcoder than stop transcoding.*/

    TSR_ENCODER_START,  /**< @brief status for encoder start */
    TSR_ENCODER_END,    /**< @brief status for encoder end */
    TSR_ENCODER_ERROR,  /**< @brief encoder error, will report to transcoder than stop transcoding. */


    TSR_WRITER_START,   /**< @brief writer start */
    TSR_WRITER_END,     /**< @brief writer end,  */
    TSR_WRITER_ERROR,   /**< @brief writer error, will report to transcoder than stop transcoding. */


    TSR_DECODER_OPEN_ERROR,
    TSR_DECODER_OPEN_SUCCESS,

    TSR_ENCODER_OEPN_ERROR,
    TSR_ENCDER_OEPN_SUCCESS,

    TSR_WRITER_OPEN_ERROR,
    TSR_WRITER_OPEN_SUCCESS,

    TSR_WRITER_CLOSE_ERROR,

    TSR_USER_CANCEL
} TranscoderError;

typedef struct VideoSourceInfo
{
    std::string filename;
    int totalFrames;
    int startFrame;
    int endFrame;

}VideoSourceInfo;

typedef struct TranscoderOption
{
    /* decoding options */
    VideoSourceInfo sourceInfo;

    /* encodidng options */
    int encodingThreadCount;
    int bitDepth;


    /* writting options */
    std::string outputFilename;
}TranscoderOption;

/*! @brief Returns a human-readable error message.
 * The string returned should not be freed.
 * @param e Error code
 * @returns NULL is the error code is unknown, otherwise a message string.
 */
std::string getTranscoderErrorStr(TranscoderError e);

#endif // TRANSCODER_COMMON_H
