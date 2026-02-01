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
			bool getPacketReceivingEnabled() const;

			// Send and ignore expected result
			template<typename Packet>
			bool sendPacketIgnoreResult(const PacketType _packetType, const Packet& _packet, const bool _reliable);

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

			// Can be used for more descriptive debug messages
			void setDebugToStringFunction(const std::function<const char*(PacketType)>&& _function) { debug.toStringFunction = _function; }

			// This signal may be useful if you need to make sure that the receive handler result gets sent first before sending another packet.
			[[nodiscard]] se::ScopedConnection connectToPostReceiveHandlerSignal(const std::function<void()>& _callback) { return postReceiveHandlerSignal.connect(_callback); }

		private:
			struct IReceiver
			{
				virtual ~IReceiver() {}
				virtual bool valid() const = 0;
				virtual void process(const Packetman& _packetman, BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) = 0;
			};
			struct IRequest
			{
				virtual ~IRequest() {}
				virtual void process(BinaryReader& _binaryReader) = 0;
				virtual bool timeout(const Time _epochNow, const Time _timeout) = 0;
				virtual bool isConnected() const = 0;
				virtual void fail() = 0;
				virtual PacketType getPacketType() const = 0;
			};
			struct Debug
			{
				static constexpr size_t RequestIdsCapacity = 32;
				std::vector<uint16_t> abandonedRequestIds;
				std::vector<uint16_t> timeoutedRequestIds;
				std::unordered_map<uint16_t, PacketType> requestIdToPacketType;
				std::function<const char* (PacketType)> toStringFunction;
			};
			void receiveHandler(BinaryReader& _binaryReader, const bool _reliable);
			uint16_t generateNextRequestId()
			{
				// Id 0: no expected result, 1: ignored result
				while (nextRequestId < 2 || isResultId(nextRequestId) || tryFind(requests, nextRequestId))
				{
					nextRequestId++;
					if (isResultId(nextRequestId))
					{
						nextRequestId = 2;
					}
				}
				return nextRequestId;
			}
			static constexpr uint16_t resultIdBit = 1 << 15;
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
				se_assert(_requestId > 1);
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
				se_assert(requestId > 1);
				return requestId;
			}
			std::string toString(const PacketType _packetType) const
			{
				std::string result;
				if (debug.toStringFunction)
				{
					result = debug.toStringFunction(_packetType);
				}
				if (result.empty())
				{
					result = std::to_string(std::underlying_type<PacketType>::type(_packetType));
				}
				return result;
			}
			std::unordered_map<PacketType, std::unique_ptr<IReceiver>> receivers;
			std::unordered_map<uint16_t, std::unique_ptr<IRequest>> requests;
			uint16_t nextRequestId = 2;
			Connection& connection;
			Signal<void()> postReceiveHandlerSignal;
			Debug debug;
		};

		template<typename PacketType>
		void Packetman<PacketType>::update()
		{
			for (typename std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::iterator it = requests.begin(); it != requests.end(); )
			{
#if SE_CONFIGURATION == SE_CONFIGURATION_FINAL_RELEASE
				constexpr std::optional<Time> timeout = Time::fromSeconds(10.0f);
#else
				constexpr std::optional<Time> timeout = std::nullopt;
#endif
				const Time epochNow = getEpochTime();
				const uint16_t requestId = it->first;
				const bool isConnected = it->second->isConnected();
				if (!isConnected || (timeout && it->second->timeout(epochNow, timeout.value())))
				{
					std::vector<uint16_t>& failedRequestIds = isConnected ? debug.timeoutedRequestIds : debug.abandonedRequestIds;
					if (failedRequestIds.size() == Debug::RequestIdsCapacity)
					{
						findAndErase(debug.requestIdToPacketType, failedRequestIds.front());
						failedRequestIds.erase(failedRequestIds.begin());
					}
					failedRequestIds.push_back(requestId);

					// Iterator might have been invalidated from the timeout callback
					it = requests.find(requestId);
					if (it != requests.end())
					{
						std::string packetTypeString;
						if (const PacketType *const packetType = tryFind(debug.requestIdToPacketType, requestId))
						{
							packetTypeString = toString(*packetType);
						}
						else
						{
							packetTypeString = "-";
						}
						const std::string reason = isConnected ? "timeout" : "abandoning";
						se::log::info("Packetman (" + connection.name + ") request removed due to " + reason + ". Id: " + std::to_string(requestId) + ", PacketType: " + packetTypeString);
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
		bool Packetman<PacketType>::getPacketReceivingEnabled() const
		{
			return connection.getPacketReceivingEnabled();
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
				se_assert(requestId != 0);
				const std::unordered_map<uint16_t, PacketType>::iterator requestIdToPacketTypeIt = debug.requestIdToPacketType.find(requestId);
				const std::unordered_map<uint16_t, std::unique_ptr<IRequest>>::iterator it = requests.find(requestId);
				if (it != requests.end())
				{
					std::unique_ptr<IRequest> request(it->second.release());
					requests.erase(it);
					if (requestIdToPacketTypeIt != debug.requestIdToPacketType.end())
					{
						debug.requestIdToPacketType.erase(requestIdToPacketTypeIt);
					}
					BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset(), _binaryReader.getSize());
					request->process(binaryReader2);
					return;
				}
				else
				{
					std::string packetTypeString;
					if (requestIdToPacketTypeIt != debug.requestIdToPacketType.end())
					{
						packetTypeString = toString(requestIdToPacketTypeIt->second);
						debug.requestIdToPacketType.erase(requestIdToPacketTypeIt);
					}
					else
					{
						packetTypeString = "-";
					}
					if (findAndErase(debug.abandonedRequestIds, requestId))
					{
						se::log::error("Request result received for a result that previously got abandoned. PacketType: " + packetTypeString);
					}
					else if (findAndErase(debug.timeoutedRequestIds, requestId))
					{
						se::log::error("Request result received for a result that previously got timeouted. PacketType: " + packetTypeString);
					}
					else
					{
						se_assert_m(false, "Request result received for an unknown result. PacketType: " + packetTypeString);
					}
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
						log::error("Failed to find receiver for reliably received PacketType: " + toString(packetType));
					}
					return;
				}
				if (!receiver->get()->valid())
				{
					if (_reliable)
					{
						log::error("Receiver is no longer valid for reliably received PacketType: " + toString(packetType));
					}
					return;
				}
				receiver->get()->process(*this, _binaryReader, _reliable, packetId, connection);
				postReceiveHandlerSignal();
				return;
			}
		}

		// Send and ignore expected result
		template<typename PacketType>
		template<typename Packet>
		bool Packetman<PacketType>::sendPacketIgnoreResult(const PacketType _packetType, const Packet& _packet, const bool _reliable)
		{
			// request id 1 is special for ignored expected result
			uint16_t packetId = 1;
			BinaryWriter binaryWriter;
			binaryWriter.serial(packetId);
			binaryWriter.serial(_packetType);
			binaryWriter.serial(_packet);
			return connection.sendPacket(binaryWriter, _reliable);
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
						: beginTime(getEpochTime())
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
					bool timeout(const Time _epochNow, const Time _timeout) final
					{
						const Time age = _epochNow - beginTime;
						if (age > _timeout)
						{
							signal(nullptr);
							return true;
						}
						else
						{
							return false;
						}
					}
					bool isConnected() const final
					{
						return !signal.isEmpty();
					}
					void fail() final
					{
						signal(nullptr);
					}
					Signal<void(Result* const)> signal;
					Time beginTime;
					PacketType packetType;
				};
				Request* const request = new Request();
				request->packetType = _packetType;
				_scopedConnection = request->signal.connect(_callback);
				debug.requestIdToPacketType[requestId] = _packetType;
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
				void process(const Packetman& _packetman, BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) final
				{
					if (_requestId != 0)
					{
						// Read PacketType from earlier for logging
						se_assert(_binaryReader.getOffset() >= sizeof(PacketType));
						BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset() - sizeof(PacketType), sizeof(PacketType));
						PacketType packetType = PacketType(0);
						binaryReader2.serial(packetType);
						log::error("The sender is expecting a result but the registered receive handler does not provide one, PacketType: " + _packetman.toString(packetType));
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
				log::error("Multiple packet receivers for the same PacketType cannot exist at the same time, PacketType: " + toString(_packetType));
				return;
			}
			struct Receiver : public IReceiver
			{
				~Receiver()
				{
					destructorCalled = true;
				}
				void process(const Packetman& _packetman, BinaryReader& _binaryReader, const bool _reliable, const uint16_t _requestId, Connection& _connection) final
				{
					se_assert(_reliable && "Packets with results should be always sent reliably");
					if (_requestId == 0)
					{
						// Read PacketType from earlier for logging
						se_assert(_binaryReader.getOffset() >= sizeof(PacketType));
						BinaryReader binaryReader2(_binaryReader.getData() + _binaryReader.getOffset() - sizeof(PacketType), sizeof(PacketType));
						PacketType packetType = PacketType(0);
						const bool packetTypeRead = binaryReader2.serial(packetType);
						(void)packetTypeRead;
						se_assert(packetTypeRead);
						log::error("The sender is not expecting a result but the registered receive handler does provide one, PacketType: " + _packetman.toString(packetType));
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

					// Request id 1 is special and indicates that the sender wants to ignore the result
					if (_requestId > 1)
					{
						const uint16_t resultId = makeResultId(_requestId);
						BinaryWriter binaryWriter;
						binaryWriter.serial(resultId);
						binaryWriter.serial(result);
						const bool sent = _connection.sendPacket(binaryWriter, true);
						(void)sent;
						se_assert(sent && "Failed to send request result");
					}
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
