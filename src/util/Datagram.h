#pragma once
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <string.h> // memcpy
#include <memory>
#include <bamboo/wire/Datagram.h>
using bamboo::sizetag_t;
using bamboo::DatagramOverflow;
#ifdef ASTRON_32BIT_DATAGRAMS
static_assert(sizeof(sizetag_t) == sizeof(int32_t),
	"sizetag_t is not 32-bit but 32-bit was requested");
#endif

#include "core/types.h"

class Datagram; // foward declaration
typedef std::shared_ptr<Datagram> DatagramPtr;
typedef std::shared_ptr<const Datagram> DatagramHandle;

// A Datagram is a buffer of binary data ready for networking (ie. formatted according to Astron's
// over-the-wire formatting specification).  It is most often used to represent Astron client and
// server messages, as well as occasionally DistributedObject field data.
class Datagram : public bamboo::Datagram
{
	protected:
		// default-constructor:
		//     creates a new datagram with some pre-allocated space
		Datagram() : bamboo::Datagram()
		{
		}

		// sized-constructor:
		//     allows you to specify the capacity of the datagram ahead of time,
		//     this should be used when the exact size is known ahead of time for performance
		Datagram(sizetag_t capacity) : bamboo::Datagram(capacity)
		{
		}

		// copy-constructor:
		//     creates a new datagram which is a deep-copy of another datagram;
		//     capacity is not perserved and instead is reduced to the size of the source datagram.
		Datagram(const Datagram &dg) : bamboo::Datagram(dg)
		{
		}

		// shallow-constructor:
		//     creates a new datagram that uses an existing buffer as its data
		Datagram(uint8_t *data, sizetag_t length, sizetag_t capacity) :
			bamboo::Datagram(data, length, capacity)
		{
		}

		// binary-constructor(pointer):
		//     creates a new datagram with a copy of the data contained at the pointer.
		Datagram(const uint8_t *data, sizetag_t length) : bamboo::Datagram(data, length)
		{
		}

		// binary-constructor(vector):
		//     creates a new datagram with a copy of the binary data contained in a vector<uint8_t>.
		Datagram(const std::vector<uint8_t> &data) : bamboo::Datagram(data)
		{
		}

		// binary-constructor(string):
		//     creates a new datagram with a copy of the data contained in a string, treated as binary.
		Datagram(const std::string &data) : bamboo::Datagram(data)
		{
		}

	public:
		static DatagramPtr create()
		{
			DatagramPtr dg_ptr(new Datagram);
			return dg_ptr;
		}
		
		static DatagramPtr create(const DatagramHandle dg)
		{
			DatagramPtr dg_ptr(new Datagram( *dg.get() ));
			return dg_ptr;
		}
		
		static DatagramPtr create(uint8_t *data, sizetag_t length, sizetag_t capacity)
		{
			DatagramPtr dg_ptr(new Datagram(data, length, capacity));
			return dg_ptr;
		}
		
		static DatagramPtr create(const uint8_t *data, sizetag_t length)
		{
			DatagramPtr dg_ptr(new Datagram(data, length));
			return dg_ptr;
		}
		
		static DatagramPtr create(const std::vector<uint8_t> &data)
		{
			DatagramPtr dg_ptr(new Datagram(data));
			return dg_ptr;
		}
		
		static DatagramPtr create(const std::string &data)
		{
			DatagramPtr dg_ptr(new Datagram(data));
			return dg_ptr;
		}
		
		static DatagramPtr create(channel_t to_channel, channel_t from_channel,
												uint16_t message_type)
		{
			Datagram *dg = new Datagram();
			dg->add_server_header(to_channel, from_channel, message_type);
			return DatagramPtr(dg);
		}
		
		static DatagramPtr create(const std::set<channel_t> &to_channels,
		                          channel_t from_channel, uint16_t message_type)
		{
			Datagram *dg = new Datagram();
			dg->add_server_header(to_channels, from_channel, message_type);
			return DatagramPtr(dg);
		}
		
		static DatagramPtr create_control(uint16_t message_type)
		{
			Datagram *dg = new Datagram(1 + sizeof(channel_t) + 2);
			dg->add_control_header(message_type);
			return DatagramPtr(dg);
		}

		void add_data(const std::vector<uint8_t>& data)
		{
			bamboo::Datagram::add_data(data);
		}
		void add_data(const std::string& str)
		{
			bamboo::Datagram::add_data(str);
		}
		void add_data(const uint8_t *data, sizetag_t length)
		{
			bamboo::Datagram::add_data(data, length);
		}
		void add_data(const DatagramHandle dg)
		{
			if(dg->buf_offset)
			{
				check_add_length(dg->buf_offset);
				memcpy(buf + buf_offset, dg->buf, dg->buf_offset);
				buf_offset += dg->buf_offset;
			}
		}

		// add_channel adds a channel to the end of the datagram.
		// Note: this method should always be used instead of add_uint64 when adding a channel
		//       to allow for future support of larger or smaller channel range limits.
		void add_channel(const channel_t &v)
		{
			check_add_length(sizeof(channel_t));
			*(channel_t *)(buf + buf_offset) = swap_le(v);
			buf_offset += sizeof(channel_t);
		}

		// add_doid adds a distributed object id to the end of the datagram.
		// Note: this method should always be used instead of add_uint32 when adding a doid
		//       to allow for future support of larger or smaller doid range limits.
		void add_doid(const doid_t &v)
		{
			check_add_length(sizeof(doid_t));
			*(doid_t *)(buf + buf_offset) = swap_le(v);
			buf_offset += sizeof(doid_t);
		}

		// add_zone adds a zone id to the end of the datagram.
		// Note: this method should always be used instead of add_uint32 when adding a zone
		//       to allow for future support of larger or smaller zone range limits.
		void add_zone(const zone_t &v)
		{
			check_add_length(sizeof(zone_t));
			*(zone_t *)(buf + buf_offset) = swap_le(v);
			buf_offset += sizeof(zone_t);
		}

		// add_location adds a parent-zone pair to the datagram; it is provided for convenience,
		// as well as code legibility and is slightly more performant than adding the parent
		// and zone separately.
		void add_location(const doid_t &parent, const zone_t &zone)
		{
			check_add_length(sizeof(doid_t) + sizeof(zone_t));
			*(doid_t *)(buf + buf_offset) = swap_le(parent);
			buf_offset += sizeof(doid_t);
			*(zone_t *)(buf + buf_offset) = swap_le(zone);
			buf_offset += sizeof(zone_t);
		}

		// add_server_header prepends a generic header for messages that are supposed to be routed
		// to one or more role instances within the server cluster. The method is provided entirely
		// for convenience.
		//
		// The format of a server header is:
		//     (uint8_t num_targets, channel_t[] targets, channel_t sender, uint16_t message_type)
		// Note that other types of datagrams do not always include a sender; therefore, the sender
		// and message_type are actually considered part of the payload of the datagram.
		void add_server_header(channel_t to, channel_t from, uint16_t message_type)
		{
			add_uint8(1);
			add_channel(to);
			add_channel(from);
			add_uint16(message_type);
		}
		void add_server_header(const std::set<channel_t> &to, channel_t from, uint16_t message_type)
		{
			add_uint8(to.size());
			for(auto it = to.begin(); it != to.end(); ++it)
			{
				add_channel(*it);
			}
			add_channel(from);
			add_uint16(message_type);
		}

		// add_control_header prepends a header for control messages that are handled by a
		// MessageDirector instance. The method is provided entirely for convenience.
		//
		// The format of a control header is:
		//     (uint8_t 1, channel_t CONTROL_MESSAGE, uint16_t message type)
		// Note that other types of datagrams optionally include a sender before the message_type;
		// therefore, the message_type is actually considered part of the payload of the datagram.
		void add_control_header(uint16_t message_type)
		{
			add_uint8(1);
			add_channel(CONTROL_MESSAGE);
			add_uint16(message_type);
		}
};
