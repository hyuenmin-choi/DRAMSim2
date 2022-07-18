/*********************************************************************************
*  Copyright (c) 2010-2011, Elliott Cooper-Balis
*                             Paul Rosenfeld
*                             Bruce Jacob
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/

#ifndef TRANSACTION_H
#define TRANSACTION_H

//Transaction.h
//
//Header file for transaction object

#include "SystemConfiguration.h"
#include "BusPacket.h"

using std::ostream; 

namespace DRAMSim
{
enum TransactionType
{
	// ## for PIM operation, modification is needed.
	// ## 구상하고 있는 방향은, bank processing unit과 rank processing unit을 각각 넣어보는 것

	/*
	
	#ifdef BANK_PROCESSING_ENABLE
	
	DATA_BPU_ACTIVATE,
	DATA_BPU_CALC,
	DATA_BPU_WRITE,
	DATA_BPU_WRITEBACK,
	DATA_BPU_READ,
	etc...

	#endif

	#ifdef RANK_PROCESSING_ENABLE

	DATA_RPU_ACTIVATE,
	DATA_RPU_CALC,
	DATA_RPU_WRITE,
	DATA_RPU_WRITEBACK,
	DATA_RPU_READ,
	etc...

	#endif

	*/
	DATA_READ,
	DATA_WRITE,
	RETURN_DATA
};

class Transaction
{
	Transaction();
public:
	//fields
	TransactionType transactionType;
	uint64_t address;
	void *data;
	uint64_t timeAdded;
	uint64_t timeReturned;

	// ## enabling transaction class can be streamed into output stream
	friend ostream &operator<<(ostream &os, const Transaction &t);
	
	//functions
	Transaction(TransactionType transType, uint64_t addr, void *data);
	Transaction(const Transaction &t);

    /* ## Bus Packet
	이전에는 memory reference, modification만 진행했으므로
	BUS에 
	
	operation signal : READ, WRITE (+page open flag) -> close page?, open page?
	state signal : Activate, precharge, refresh -> ?? 공부해야함...
	result data : DATA
	
	를 실어 보내면 됨.

	In PIM version,
	memory processing unit가 read, write 뿐만아니라 
	internal computation + user define cheap computation 할 수 있음
	rough bus packet structure

	Elaboration : 
	1. Processing unit한테 일일이 모든 instr을 bus packet으로 줘야하나?
	아니면 rough한 instr 하나에 processing unit이 complex한 operation을 하게 만들어야 하나?
	ex) PIM_READ -> PIM_CALC -> PIM_WRITE vs PIM_PROCESS => semantic is same

	2. Processing unit에 별도의 storage(PU reg)를 만들어야할 것 같음.

	general operation signal : READ_FROM_PU, READ_FROM_MEM, WRITE 아마 원래 dram bus packet이랑 똑같이
	PIM specific operation signal :
	READ_BY_PU
	, WRITE_BY_PU(for internal computation, by processing unit?) -> writeback, writeall
	, OPERATION(아마 더 specific하게 갈 거 같음)
	, 

	result data : DATA_RAW, DATA_PROCESSED
	*/
	BusPacketType getBusPacketType()
	{
		switch (transactionType)
		{
			case DATA_READ:
			if (rowBufferPolicy == ClosePage)
			{
				return READ_P;
			}
			else if (rowBufferPolicy == OpenPage)
			{
				return READ; 
			}
			else
			{
				ERROR("Unknown row buffer policy");
				abort();
			}
			break;
		case DATA_WRITE:
			if (rowBufferPolicy == ClosePage)
			{
				return WRITE_P;
			}
			else if (rowBufferPolicy == OpenPage)
			{
				return WRITE; 
			}
			else
			{
				ERROR("Unknown row buffer policy");
				abort();
			}
			break;
		default:
			ERROR("This transaction type doesn't have a corresponding bus packet type");
			abort();
		}
	}
};

}

#endif

