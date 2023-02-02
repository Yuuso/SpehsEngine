#pragma once

#include <functional>
#include "boost/signals2/signal.hpp"
#include "SpehsEngine/Net/Connection2.h"


namespace se
{
	namespace net
	{
		// Handles receiving and forwarding packets with optional responses
		template<typename PacketType>
		class Packetman
		{
		public:

			Packetman(Connection2& _connection);

			void update();

			// Send without result
			template<typename Packet>
			bool sendPacket(const PacketType _packetType, const Packet& _packet, const bool _reliable);

			// Send with result
			template<typename Packet, typename Result>
			bool sendPacket(const PacketType _packetType, const Packet& _packet,
				boost::signals2::scoped_connection& _scopedConnection, const std::function<void(Result* const)>& _callback);

			// Receive without result
			template<typename Packet>
			void registerReceiveHandler(const PacketType _packetType,
				boost::signals2::scoped_connection& _scopedConnection, const std::function<void(Packet&, const bool)>& _receiveHandler);

			// Receive with result
			template<typename Packet, typename Result>
			void registerReceiveHandler(const PacketType _packetType,
				boost::signals2::scoped_connection& _scopedConnection, const std::function<Result(Packet&, const bool)>& _receiveHandler);

		private:
			struct IReceiver
			{
				virtual ~IReceiver() {}
				virtual void process(se::ReadBuffer& _readBuffer, const bool _reliable, const uint16_t _requestId, Connection2& _connection) = 0;
				virtual bool valid() const = 0;
			};
			struct IRequest
			{
				virtual ~IRequest() {}
				virtual void process(se::ReadBuffer& _readBuffer) = 0;
				virtual bool timeout(const time::Time& timeout) = 0;
			};
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
			Connection2& connection;
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
		Packetman<PacketType>::Packetman(Connection2& _connection)
			: connection(_connection)
		{
			connection.setReceiveHandler(
				[this](se::ReadBuffer& _readBuffer, const bool _reliable)
				{
					uint16_t packetId = 0;
					if (!_readBuffer.read(packetId))
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
							se::ReadBuffer readBuffer2(_readBuffer[_readBuffer.getOffset()], _readBuffer.getSize());
							it->second->process(readBuffer2);
							requests.erase(it);
						}
					}
					else
					{
						// Receive packet
						se_assert(isRequestId(packetId));
						PacketType packetType = PacketType(0);
						if (!_readBuffer.read(packetType))
						{
							se_assert(false && "Failed to read PacketType");
							return;
						}
						std::unique_ptr<IReceiver>* const receiver = tryFind(receivers, packetType);
						if (!receiver)
						{
							se_assert(!_reliable && "Failed to find receiver for reliably sent PacketType");
							return;
						}
						if (!receiver->get()->valid())
						{
							se_assert(!_reliable && "Receiver for reliably sent PacketType is no longer valid");
							return;
						}
						se::ReadBuffer readBuffer2(_readBuffer[_readBuffer.getOffset()], _readBuffer.getBytesRemaining());
						receiver->get()->process(readBuffer2, _reliable, packetId, connection);
					}
				});
		}

		// Send without result
		template<typename PacketType>
		template<typename Packet>
		bool Packetman<PacketType>::sendPacket(const PacketType _packetType, const Packet& _packet, const bool _reliable)
		{
			uint16_t packetId = 0;
			WriteBuffer writeBuffer;
			se_write(writeBuffer, packetId);
			se_write(writeBuffer, _packetType);
			se_write(writeBuffer, _packet);
			return connection.sendPacket(writeBuffer, _reliable);
		}

		// Send with result
		template<typename PacketType>
		template<typename Packet, typename Result>
		bool Packetman<PacketType>::sendPacket(const PacketType _packetType, const Packet& _packet,
			boost::signals2::scoped_connection& _scopedConnection, const std::function<void(Result* const)>& _callback)
		{
			const uint16_t requestId = generateNextRequestId();
			WriteBuffer writeBuffer;
			se_write(writeBuffer, requestId);
			se_write(writeBuffer, _packetType);
			se_write(writeBuffer, _packet);
			if (connection.sendPacket(writeBuffer, true))
			{
				struct Request : public IRequest
				{
					Request()
						: beginTime(time::now())
					{
					}
					void process(se::ReadBuffer& _readBuffer) final
					{
						Result result;
						if (_readBuffer.read(result))
						{
							signal(&result);
						}
						else
						{
							se_assert(false && "Failed to read result as the expected type");
						}
					}
					bool timeout(const time::Time& timeout) final
					{
						if (signal.empty())
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
					boost::signals2::signal<void(Result* const)> signal;
					time::Time beginTime;
				};
				Request* const request = new Request();
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
		void Packetman<PacketType>::registerReceiveHandler(const PacketType _packetType, boost::signals2::scoped_connection& _scopedConnection, const std::function<void(Packet&, const bool)>& _receiveHandler)
		{
			std::unique_ptr<IReceiver>& iReceiver = receivers[_packetType];
			if (iReceiver && iReceiver->valid())
			{
				se_assert(false && "Multiple packet receivers for the same PacketType cannot exist at the same time");
				return;
			}
			struct Receiver : public IReceiver
			{
				void process(se::ReadBuffer& _readBuffer, const bool _reliable, const uint16_t _requestId, Connection2& _connection) final
				{
					se_assert(_requestId == 0 && "The sender is expecting a result but the registered receive handler does not provide one");
					Packet packet;
					if (_readBuffer.read(packet))
					{
						signal(packet, _reliable);
					}
					else
					{
						se_assert(false && "Failed to read packet");
					}
				}
				bool valid() const final
				{
					return !signal.empty();
				}
				boost::signals2::signal<void(Packet&, const bool)> signal;
			};
			Receiver* const receiver = new Receiver();
			_scopedConnection = receiver->signal.connect(_receiveHandler);
			iReceiver.reset(receiver);
		}

		// Receive with result
		template<typename PacketType>
		template<typename Packet, typename Result>
		void Packetman<PacketType>::registerReceiveHandler(const PacketType _packetType, boost::signals2::scoped_connection& _scopedConnection, const std::function<Result(Packet&, const bool)>& _receiveHandler)
		{
			std::unique_ptr<IReceiver>& iReceiver = receivers[_packetType];
			if (iReceiver && iReceiver->valid())
			{
				se_assert(false && "Multiple packet receivers for the same PacketType cannot exist at the same time");
				return;
			}
			struct Receiver : public IReceiver
			{
				void process(se::ReadBuffer& _readBuffer, const bool _reliable, const uint16_t _requestId, Connection2& _connection) final
				{
					se_assert(_requestId != 0 && "The sender is not expecting a result but the registered receive handler does provide one")
						se_assert(_reliable && "Packets with results should be always sent reliably");
					Result result;
					Packet packet;
					if (_readBuffer.read(packet))
					{
						result = callback(packet, _reliable);
					}
					else
					{
						se_assert(false && "Failed to read packet");
					}
					const uint16_t resultId = makeResultId(_requestId);
					WriteBuffer writeBuffer;
					se_write(writeBuffer, resultId);
					se_write(writeBuffer, result);
					const bool sent = _connection.sendPacket(writeBuffer, true);
					(void)sent;
					se_assert(sent && "Failed to send request result");
				}
				bool valid() const final
				{
					return !signal.empty();
				}
				boost::signals2::signal<void()> signal;
				std::function<Result(Packet&, const bool)> callback;
			};
			Receiver* const receiver = new Receiver();
			receiver->callback = _receiveHandler;
			_scopedConnection = receiver->signal.connect([]() {});
			iReceiver.reset(receiver);
		}
	}
}
