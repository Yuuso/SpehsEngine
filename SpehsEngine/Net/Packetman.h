#pragma once

#include "SpehsEngine/Net/Connection.h"


namespace se
{
	namespace net
	{
		// Handles receiving and forwarding packets with optional responses
		template<typename PacketType>
		class Packetman
		{
		public:

			Packetman(Connection& _connection);
			~Packetman();

			void update();

			void setPacketReceivingEnabled(const bool _enabled);

			// Send without result
			template<typename Packet>
			bool sendPacket(const PacketType _packetType, const Packet& _packet, const bool _reliable);

			// Send with result
			template<typename Packet, typename Result>
			bool sendPacket(const PacketType _packetType, const Packet& _packet,
				ScopedConnection& _scopedConnection, const std::function<void(Result* const)>& _callback);

			// Receive without result
			template<typename Packet>
			void registerReceiveHandler(const PacketType _packetType,
				ScopedConnection& _scopedConnection, const std::function<void(Packet&, const bool)>& _receiveHandler);

			// Receive with result
			template<typename Packet, typename Result>
			void registerReceiveHandler(const PacketType _packetType,
				ScopedConnection& _scopedConnection, const std::function<Result(Packet&, const bool)>& _receiveHandler);

			// Returns true if there is at least one sent packet of the given packet type still awaiting for a result
			bool hasUnfinishedResults(const PacketType _packetType) const;

		private:
			struct IReceiver
			{
				virtual ~IReceiver() {}
				virtual bool valid() const = 0;
				virtual void process(BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) = 0;
			};
			struct IRequest
			{
				virtual ~IRequest() {}
				virtual void process(BinaryReader& _binaryReader) = 0;
				virtual bool timeout(const time::Time& timeout) = 0;
				virtual void fail() = 0;
				virtual PacketType getPacketType() const = 0;
			};
			void receiveHandler(BinaryReader& _binaryReader, const bool _reliable);
			uint16_t generateNextRequestId()
			{
				while (nextRequestId == 0 || isResultId(nextRequestId) || tryFind(requests, nextRequestId))
				{
					nextRequestId++;
					if (isResultId(nextRequestId))
					{
						nextRequestId = 1;
					}
				}
				return nextRequestId;
			}
			static const uint16_t resultIdBit = 1 << 15;
			static uint16_t isRequestId(const uint16_t _requestId)
			{
				return !checkBit(_requestId, resultIdBit);
			}
			static uint16_t isResultId(const uint16_t _requestId)
			{
				return checkBit(_requestId, resultIdBit);
			}
			static uint16_t makeResultId(const uint16_t _requestId)
			{
				se_assert(_requestId != 0);
				se_assert(!isResultId(_requestId));
				uint16_t resultId = _requestId;
				enableBit(resultId, resultIdBit);
				return resultId;
			}
			static uint16_t makeRequestId(const uint16_t _resultId)
			{
				se_assert(isResultId(_resultId));
				uint16_t requestId = _resultId;
				disableBit(requestId, resultIdBit);
				se_assert(requestId != 0);
				return requestId;
			}
			std::unordered_map<PacketType, std::unique_ptr<IReceiver>> receivers;
			std::unordered_map<uint16_t, std::unique_ptr<IRequest>> requests;
			uint16_t nextRequestId = 1;
			Connection& connection;
		};

		template<typename PacketType>
		void Packetman<PacketType>::update()
		{
			for (typename std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::iterator it = requests.begin(); it != requests.end(); )
			{
#if SE_CONFIGURATION == SE_CONFIGURATION_FINAL_RELEASE
				const time::Time timeout = time::fromSeconds(10.0f);
#else
				const time::Time timeout = time::fromSeconds(99999.9f);
#endif
				const uint16_t requestId = it->first;
				if (it->second->timeout(timeout))
				{
					// Iterator might have been invalidated from the timeout callback
					it = requests.find(requestId);
					if (it != requests.end())
					{
						it = requests.erase(it);
					}
					else
					{
						break;
					}
				}
				else
				{
					it++;
				}
			}
		}

		template<typename PacketType>
		void Packetman<PacketType>::setPacketReceivingEnabled(const bool _enabled)
		{
			connection.setPacketReceivingEnabled(_enabled);
		}

		template<typename PacketType>
		Packetman<PacketType>::Packetman(Connection& _connection)
			: connection(_connection)
		{
			connection.setReceiveHandler(std::bind(&Packetman<PacketType>::receiveHandler, this, std::placeholders::_1, std::placeholders::_2));
		}

		template<typename PacketType>
		Packetman<PacketType>::~Packetman()
		{
			// Fail all the remaining requests
			std::unordered_map<uint16_t, std::unique_ptr<IRequest>> temp;
			std::swap(temp, requests);
			for (typename std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::iterator it = temp.begin(); it != temp.end(); it++)
			{
				it->second->fail();
			}
			se_assert(requests.empty() && "New requests should not be added during the destructor");
		}

		template<typename PacketType>
		void Packetman<PacketType>::receiveHandler(BinaryReader& _binaryReader, const bool _reliable)
		{
			uint16_t packetId = 0;
			if (!_binaryReader.serial(packetId))
			{
				se_assert(false && "Failed to read packet id");
				return;
			}
			if (isResultId(packetId))
			{
				// Receive result
				const uint16_t requestId = makeRequestId(packetId);
				const std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::iterator it = requests.find(requestId);
				se_assert(it != requests.end());
				if (it != requests.end())
				{
					std::unique_ptr<IRequest> request(it->second.release());
					requests.erase(it);
					BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset(), _binaryReader.getSize());
					request->process(binaryReader2);
					return;
				}
			}
			else
			{
				// Receive packet
				se_assert(isRequestId(packetId));
				PacketType packetType = PacketType(0);
				if (!_binaryReader.serial(packetType))
				{
					if (_reliable)
					{
						log::error("Failed to read PacketType");
					}
					return;
				}
				std::unique_ptr<IReceiver>* const receiver = tryFind(receivers, packetType);
				if (!receiver)
				{
					if (_reliable)
					{
						log::error("Failed to find receiver for reliably received PacketType: " + std::to_string(int(packetType)));
					}
					return;
				}
				if (!receiver->get()->valid())
				{
					if (_reliable)
					{
						log::error("Receiver is no longer valid for reliably received PacketType: " + std::to_string(int(packetType)));
					}
					return;
				}
				receiver->get()->process(_binaryReader, _reliable, packetId, connection);
				return;
			}
		}

		// Send without result
		template<typename PacketType>
		template<typename Packet>
		bool Packetman<PacketType>::sendPacket(const PacketType _packetType, const Packet& _packet, const bool _reliable)
		{
			uint16_t packetId = 0;
			BinaryWriter binaryWriter;
			binaryWriter.serial(packetId);
			binaryWriter.serial(_packetType);
			binaryWriter.serial(_packet);
			return connection.sendPacket(binaryWriter, _reliable);
		}

		// Send with result
		template<typename PacketType>
		template<typename Packet, typename Result>
		bool Packetman<PacketType>::sendPacket(const PacketType _packetType, const Packet& _packet,
			ScopedConnection& _scopedConnection, const std::function<void(Result* const)>& _callback)
		{
			const uint16_t requestId = generateNextRequestId();
			BinaryWriter binaryWriter;
			binaryWriter.serial(requestId);
			binaryWriter.serial(_packetType);
			binaryWriter.serial(_packet);
			if (connection.sendPacket(binaryWriter, true))
			{
				struct Request : public IRequest
				{
					Request()
						: beginTime(time::now())
					{
					}
					PacketType getPacketType() const final
					{
						return packetType;
					}
					void process(BinaryReader& _binaryReader) final
					{
						Result result;
						if (_binaryReader.serial(result))
						{
							signal(&result);
							return;
						}
						else
						{
							se_assert(false && "Failed to read result as the expected type");
						}
					}
					bool timeout(const time::Time& timeout) final
					{
						if (signal.isEmpty())
						{
							return true;
						}
						const time::Time age = time::now() - beginTime;
						if (age > timeout)
						{
							signal(nullptr);
							return true;
						}
						else
						{
							return false;
						}
					}
					void fail() final
					{
						signal(nullptr);
					}
					Signal<void(Result* const)> signal;
					time::Time beginTime;
					PacketType packetType;
				};
				Request* const request = new Request();
				request->packetType = _packetType;
				_scopedConnection = request->signal.connect(_callback);
				std::unique_ptr<IRequest>& iRequest = requests[requestId];
				se_assert(!iRequest);
				iRequest.reset(request);
				return true;
			}
			else
			{
				return false;
			}
		}

		// Receive without result
		template<typename PacketType>
		template<typename Packet>
		void Packetman<PacketType>::registerReceiveHandler(const PacketType _packetType,
			ScopedConnection& _scopedConnection, const std::function<void(Packet&, const bool)>& _receiveHandler)
		{
			std::unique_ptr<IReceiver>& iReceiver = receivers[_packetType];
			if (iReceiver && iReceiver->valid())
			{
				se_assert(false && "Multiple packet receivers for the same PacketType cannot exist at the same time");
				return;
			}
			struct Receiver : public IReceiver
			{
				void process(BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) final
				{
					if (_requestId != 0)
					{
						// Read PacketType from earlier for logging
						se_assert(_binaryReader.getOffset() >= sizeof(PacketType));
						BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset() - sizeof(PacketType), sizeof(PacketType));
						PacketType packetType = PacketType(0);
						binaryReader2.serial(packetType);
						log::error("The sender is expecting a result but the registered receive handler does not provide one, PacketType: " + std::to_string(int(packetType)));
					}
					Packet packet;
					if (_binaryReader.serial(packet))
					{
						signal(packet, _reliable);
						return;
					}
					else
					{
						se_assert(false && "Failed to read packet");
					}
				}
				bool valid() const final
				{
					return !signal.isEmpty();
				}
				Signal<void(Packet&, const bool)> signal;
			};
			Receiver* const receiver = new Receiver();
			_scopedConnection = receiver->signal.connect(_receiveHandler);
			iReceiver.reset(receiver);
		}

		// Receive with result
		template<typename PacketType>
		template<typename Packet, typename Result>
		void Packetman<PacketType>::registerReceiveHandler(const PacketType _packetType,
			ScopedConnection& _scopedConnection, const std::function<Result(Packet&, const bool)>& _receiveHandler)
		{
			std::unique_ptr<IReceiver>& iReceiver = receivers[_packetType];
			if (iReceiver && iReceiver->valid())
			{
				se_assert(false && "Multiple packet receivers for the same PacketType cannot exist at the same time");
				return;
			}
			struct Receiver : public IReceiver
			{
				~Receiver()
				{
					destructorCalled = true;
				}
				void process(BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) final
				{
					se_assert(_reliable && "Packets with results should be always sent reliably");
					if (_requestId == 0)
					{
						// Read PacketType from earlier for logging
						se_assert(_binaryReader.getOffset() >= sizeof(PacketType));
						BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset() - sizeof(PacketType), sizeof(PacketType));
						PacketType packetType = PacketType(0);
						se_assert(binaryReader2.serial(packetType));
						log::error("The sender is not expecting a result but the registered receive handler does provide one, PacketType: " + std::to_string(int(packetType)));
					}

					Result result;
					Packet packet;
					if (_binaryReader.serial(packet))
					{
						// NOTE: the callback should not deallocate this (and the _connection).
						result = callback(packet, _reliable);
						if (destructorCalled)
						{
							log::error("The receive handler should not deallocate Packetman. Result packet will not be sent.");
							return;
						}
					}
					else
					{
						se_assert(false && "Failed to read packet");
					}

					const uint16_t resultId = makeResultId(_requestId);
					BinaryWriter binaryWriter;
					binaryWriter.serial(resultId);
					binaryWriter.serial(result);
					const bool sent = _connection.sendPacket(binaryWriter, true);
					(void)sent;
					se_assert(sent && "Failed to send request result");
				}
				bool valid() const final
				{
					return !signal.isEmpty();
				}
				Signal<void()> signal;
				std::function<Result(Packet&, const bool)> callback;
				bool destructorCalled = false;
			};
			Receiver* const receiver = new Receiver();
			receiver->callback = _receiveHandler;
			_scopedConnection = receiver->signal.connect([]() {});
			iReceiver.reset(receiver);
		}

		template<typename PacketType>
		bool Packetman<PacketType>::hasUnfinishedResults(const PacketType _packetType) const
		{
			for (typename std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::const_iterator it = requests.begin(); it != requests.end(); it++)
			{
				if (it->second.get()->getPacketType() == _packetType)
				{
					return true;
				}
			}
			return false;
		}
	}
}
