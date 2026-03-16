/* --COPYRIGHT--,BSD
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <HAL.h>
#include <GUI_mpack.h>
#include <include/GUIComm.h>
#include <include/QmathLib.h>
#include "src/MSP430_GUI/mpack/mpack.h"


#ifndef INCLUDE_CALLBACKS_H_
#define INCLUDE_CALLBACKS_H_

// Global variables for the callbacks
volatile bool bEnableSwitch;
volatile _q qIncrement;

// Functions to receive data from GUI
extern void GUICallback_QMathData(mpack_tag_t *tag, mpack_reader_t *reader);
extern void GUICallback_boolEnable(mpack_tag_t *tag, mpack_reader_t *reader);

// Functions to read data from mpack
inline extern uint64_t GUICallback_ReadUInt(mpack_tag_t *tag)
{
    return mpack_tag_uint_value(tag);
}

inline extern int64_t GUICallback_ReadInt(mpack_tag_t *tag)
{
    return mpack_tag_int_value(tag);
}

inline extern bool GUICallback_ReadBool(mpack_tag_t *tag)
{
    return mpack_tag_bool_value(tag);
}

inline extern float GUICallback_ReadFloat(mpack_tag_t *tag)
{
    return mpack_tag_float_value(tag);
}

inline extern double GUICallback_ReadDouble(mpack_tag_t *tag)
{
    return mpack_tag_double_value(tag);
}

#endif /* INCLUDE_CALLBACKS_H_ */
