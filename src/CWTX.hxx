/*
Copyright (c) 2012,2013,2014 Matthew H. Reilly (kb1vc)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CWTX_HDR
#define CWTX_HDR
#include "SoDaBase.hxx"
#include "MultiMBox.hxx"
#include "Command.hxx"
#include "Params.hxx"
#include "UI.hxx"
#include "CWGenerator.hxx"

#include <time.h>
#include <sys/time.h>

namespace SoDa {
  /**
   * The CW text-to-envelope converter run loop
   *
   * This block uses the CWGenerator object to convert text to a
   * CW envelope. 
   */
  class CWTX : public SoDaThread {
  public:
    /**
     * @brief Constructor
     * @param params block describing intial setup of the radio
     * @param cwtxt_stream stream carrying text strings to be converted to morse code
     * @param cw_env_stream envelope stream from text-to-CW converter
     * @param cmd_stream command stream
     */
    CWTX(Params * params, CmdMBox * cwtxt_stream, DatMBox * cw_env_stream, CmdMBox * cmd_stream);
    /**
     * @brief CWTX run loop: translate text to CW envelopes, handle incoming commands
     */
    void run();
  private:
    /**
     * @brief execute GET commands from the command channel
     * @param cmd the incoming command
     */
    void execGetCommand(Command * cmd); 
    /**
     * @brief handle SET commands from the command channel
     * @param cmd the incoming command
     */
    void execSetCommand(Command * cmd); 
    /**
     * @brief handle Report commands from the command channel
     * @param cmd the incoming command
     */
    void execRepCommand(Command * cmd);

    /**
     * @brief add text to the outbound text queue
     * @param buf zero terminated sequence of ASCII characters
     */
    void enqueueText(const char * buf);

    /**
     * @brief forget all enqueued text
     */
    void clearTextQueue();

    /**
     * @brief if a character is available, encode it into an envelope
     */
    bool sendAvailChar();

    int sent_char_count; 

    CWGenerator * cwgen; ///< Pointer to a text-to-morse translator
    
    CmdMBox * cwtxt_stream; ///< stream of characters to be encoded (from UI or elsewhere)
    CmdMBox * cmd_stream; ///< stream of commands to modify radio state
    DatMBox * cw_env_stream; ///< stream carrying cw envelope buffers to USRPTX
    unsigned int cwtxt_subs; ///< subscription for text stream
    unsigned int cmd_subs; ///< subscription for command stream

    bool txmode_is_cw; ///< if true, we're transmitting a CW stream
    bool old_txmode_is_cw; ///< remember the mode we were in
    bool tx_on; ///< if true, we're transmitting a CW stream or a beacon
    
    double rf_sample_rate; ///< samples/sec for generating the envelope
    unsigned int rf_buffer_size; ///< the size of the envelope buffer

    float * beacon_envelope; ///< the currently generated envelope

    std::queue<char> text_queue; ///< characters waiting to be sent
    boost::mutex text_lock; ///< lock for text_queue
    
    std::queue<int> break_notification_id_queue;  ///< tags inserted into text stream -- send tag when the CW envelope generator gets to this character.
    boost::mutex break_id_lock; ///< lock for break_notification queue
  }; 
}


#endif
