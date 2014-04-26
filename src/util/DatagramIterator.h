#pragma once
#include "Datagram.h"
#include <bamboo/wire/DatagramIterator.h>
using bamboo::DatagramIteratorEOF;

#include <bamboo/module/Struct.h>
#include <bamboo/module/Method.h>
#include <bamboo/module/Field.h>
#include <bamboo/module/Parameter.h>
#ifdef _DEBUG
#include <fstream>
#endif

// A DatagramIterator lets you step trough a datagram by reading a single value at a time.
class DatagramIterator : public bamboo::DatagramIterator
{
	protected:
		DatagramHandle m_dg;

	public:
		// constructor
		DatagramIterator(DatagramHandle dg, sizetag_t offset = 0) :
			bamboo::DatagramIterator(dg.get(), offset), m_dg(dg)
		{
		}

		// read_channel reads a channel_t from the datagram.
		channel_t read_channel()
		{
			check_read_length(sizeof(channel_t));
			channel_t r = *(channel_t*)(m_dg->get_data() + m_offset);
			m_offset += sizeof(channel_t);
			return swap_le(r);
		}

		// read_doid reads a doid_t from the datagram.
		doid_t read_doid()
		{
			check_read_length(sizeof(doid_t));
			doid_t r = *(doid_t*)(m_dg->get_data() + m_offset);
			m_offset += sizeof(doid_t);
			return swap_le(r);
		}

		// read_zone reads a zone_t from the datagram.
		zone_t read_zone()
		{
			check_read_length(sizeof(zone_t));
			zone_t r = *(zone_t*)(m_dg->get_data() + m_offset);
			m_offset += sizeof(zone_t);
			return swap_le(r);
		}

		// read_datagram reads a blob from the datagram and returns it as another datagram.
		DatagramPtr read_datagram()
		{
			sizetag_t length = read_size();
			return Datagram::create(m_dg->get_data() + m_offset, length);
		}

		// get_recipient_count returns the datagram's recipient count. Does not advance the offset.
		// Should be used when the current offset needs to be saved and/or if the next field in the
		// datagram is not the recipient_count. If stepping through a fresh datagram, use read_uint8.
		uint8_t get_recipient_count() const
		{
			if(m_dg->size() > 0)
			{
				return *(uint8_t*)(m_dg->get_data());
			}
			else
			{
				throw DatagramIteratorEOF("Cannot read header from empty datagram.");
			}
		}

		// get_sender returns the datagram's sender. Does not advance the offset.
		// Should be used when the current offset needs to be saved and/or if the next field in the
		//     datagram is not the sender. If stepping through a fresh datagram, use read_channel.
		channel_t get_sender()
		{
			sizetag_t offset = m_offset; // save offset

			m_offset = 1 + get_recipient_count() * sizeof(channel_t); // seek sender
			channel_t sender = read_channel(); // read sender

			m_offset = offset; // restore offset
			return sender;
		}

		// get_msg_type returns the datagram's message type. Does not advance the offset.
		// Should be used when the current offset needs to be saved and/or if the next field in the
		//     datagram is not the msg_type. If stepping through a fresh datagram, use read_uint16.
		uint16_t get_msg_type()
		{
			sizetag_t offset = m_offset; // save offset

			m_offset = 1 + get_recipient_count() * sizeof(channel_t) + sizeof(channel_t); // seek message type
			uint16_t msg_type = read_uint16(); // read message type

			m_offset = offset; // restore offset
			return msg_type;
		}

		// seek_payload seeks to immediately after the list of receivers
		//     (typically, [channel_t sender, uint16_t msgtype, ...])
		void seek_payload()
		{
			m_offset = 0; // Seek to start
			m_offset = 1 + get_recipient_count() * sizeof(channel_t);
		}
};
