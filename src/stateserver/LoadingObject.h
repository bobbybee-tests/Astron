#pragma once
#include "DBStateServer.h"
#include "DistributedObject.h"

class LoadingObject : public MDParticipantInterface
{
		friend class DBStateServer;

	public:
		LoadingObject(DBStateServer *stateserver, doid_t do_id, doid_t parent_id, zone_t zone_id,
		              const std::unordered_set<uint32_t> &contexts = std::unordered_set<uint32_t>());
		LoadingObject(DBStateServer *stateserver, doid_t do_id, doid_t parent_id, zone_t zone_id,
		              const bamboo::Class *dclass, DatagramIterator &dgi,
		              const std::unordered_set<uint32_t> &contexts = std::unordered_set<uint32_t>());
		~LoadingObject();

		void begin();

		virtual void handle_datagram(DatagramHandle in_dg, DatagramIterator &dgi);

	private:
		DBStateServer *m_dbss;
		doid_t m_do_id;
		doid_t m_parent_id;
		zone_t m_zone_id;
		uint32_t m_context;
		LogCategory *m_log;

		// Upstream object data
		const bamboo::Class *m_dclass;
		std::unordered_map<const bamboo::Field*, std::vector<uint8_t> > m_field_updates;
		std::unordered_map<const bamboo::Field*, std::vector<uint8_t> > m_required_fields;
		std::map<const bamboo::Field*, std::vector<uint8_t> > m_ram_fields;
		std::unordered_set<uint32_t> m_valid_contexts;

		// Received datagrams while waiting for reply
		std::list<DatagramHandle> m_datagram_queue;
		bool m_is_loaded;

		// send_get_object makes the initial request to the database for the object data
		void inline send_get_object(doid_t do_id);
		// replay_datagrams while replay the datagrams for a loaded distributed object
		void inline replay_datagrams(DistributedObject* obj);
		// forward_datagrams will replay the datagrams to the dbss for a failed load
		void inline forward_datagrams();
};
