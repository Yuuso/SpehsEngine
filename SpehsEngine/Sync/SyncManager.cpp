#include "stdafx.h"
#include "SpehsEngine/Sync/SyncManager.h"

#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/SocketTCP.h"

namespace se
{
	namespace sync
	{
		static int debugLevel = 0;
		
		Manager::Manager(net::SocketTCP& _socket)
			: socket(_socket)
		{

		}

		Manager::~Manager()
		{
			for (size_t i = 0; i < entries.size(); i++)
				delete entries[i];
			entries.clear();
		}

		static const uint64_t syncManagerInitializeTypesMagicNumber = 0x13372112BEEF0001;
		static const uint64_t syncManagerInitializeTypesResponseMagicNumber = 0x13372112BEEF0002;

		bool Manager::initialize()
		{
			if (initialized)
			{
				se::log::error("se::sync::Manager is already initialized.");
				return false;
			}

			if (!socket.isConnected())
			{
				se::log::error("se::sync::Manager cannot initialize, the provided socket is not in the connected state.");
				return false;
			}

			if (socket.isReceiving())
			{
				se::log::error("se::sync::Manager cannot initialize, the provided socket is already in the receiving state.");
				return false;
			}

			//Clear all instances
			for (size_t i = 0; i < entries.size(); i++)
				delete entries[i];
			entries.clear();

			//Reset the remote sync manager state
			remoteManager.typeCompatibility = TypeCompatibility::unknown;
			remoteManager.typeCompatibilityResponse = TypeCompatibility::unknown;

			//Print my registered type info(?)
			if (debugLevel >= 1)
			{
				se::log::info("se::sync::Manager registered types:");
				for (size_t i = 0; i < registeredTypes.size(); i++)
				{
					se::log::info("[" + std::to_string(i) + "]\tLocal: name: " + registeredTypes[i].local.name + " typeid: " + registeredTypes[i].local.typeId);
					se::log::info("\tRemote: name: " + registeredTypes[i].remote.name + " typeid: " + registeredTypes[i].remote.typeId);
				}
			}

			//Prepare a packet for the remote sync manager
			WriteBuffer buffer;
			if (debugLevel >= 1)
				se::log::info("se::sync::Manager: writing an init packet for the remote counterpart...");
			//Write a header value for some packet verification
			buffer.write(PacketType::types);
			buffer.write(syncManagerInitializeTypesMagicNumber);
			//Write my registered types
			const size_t registeredTypesCount = registeredTypes.size();
			buffer.write(registeredTypesCount);
			for (size_t i = 0; i < registeredTypesCount; i++)
				buffer.write(registeredTypes[i]);
			//Send
			socket.sendPacket(buffer);
			if (debugLevel >= 1)
				se::log::info("se::sync::Manager: sending an init packet for the remote counterpart...");
			//Start receiving
			socket.setOnReceiveCallback(std::bind(&Manager::receiveHandler, this, std::placeholders::_1));

			//Wait...
			const se::time::Time begin = se::time::now();
			const se::time::Time timeout = se::time::fromSeconds(999999.0f);// 10.0f);
			while (remoteManager.typeCompatibility == TypeCompatibility::unknown || remoteManager.typeCompatibilityResponse == TypeCompatibility::unknown)
			{
				socket.update();
				if (se::time::now() - begin >= timeout)
				{//Timeouted
					if (debugLevel >= 1)
						se::log::info("se::sync::Manager: initialize timeouted. The remote sync manager did not respond within the given time limit");
					socket.setOnReceiveCallback(std::function<void(ReadBuffer&)>());
					return false;
				}
			}

			//Inspect results
			if (remoteManager.typeCompatibility == TypeCompatibility::compatible)
				se::log::info("se::sync::Manager: compatible with the remote sync manager's types.");
			else
				se::log::info("se::sync::Manager: incompatible with the remote sync manager's types.");
			if (remoteManager.typeCompatibilityResponse == TypeCompatibility::compatible)
				se::log::info("se::sync::Manager: remote sync manager is compatible with my types.");
			else
				se::log::info("se::sync::Manager: remote sync manager is incompatible with my types.");
			if (remoteManager.typeCompatibility == TypeCompatibility::compatible && remoteManager.typeCompatibilityResponse == TypeCompatibility::compatible)
			{
				se::log::info("se::sync::Manager: initialization complete.");
				initialized = true;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool Manager::receiveHandler(ReadBuffer& buffer)
		{
			while (buffer.getBytesRemaining())
			{
				PacketType packetType;
				buffer.read(packetType);
				if (!isInitialized() && packetType != PacketType::types)
				{
					se::log::warning("se::sync::Manager: first received packet type is expected to contain type data! Packet type was set to: " + std::to_string((int)packetType));
					return true;
				}

				switch (packetType)
				{
				default:
					se::log::warning("se::sync::Manager: received unknown packet type: " + std::to_string((int)packetType));
					return true;
				case PacketType::types:
				{//Not initialized
					uint64_t magic;
					buffer.read(magic);
					if (magic == syncManagerInitializeTypesMagicNumber)
					{//Received remote types
						size_t remoteRegisteredTypesCount;
						buffer.read(remoteRegisteredTypesCount);
						std::vector<TypeInfo> remoteRegisteredTypes(remoteRegisteredTypesCount);
						remoteManager.typeCompatibility = TypeCompatibility::compatible;
						std::string failureString;
						for (size_t r = 0; r < remoteRegisteredTypesCount; r++)
						{
							buffer.read(remoteRegisteredTypes[r]);
							const TypeInfo& remoteTypeInfo = remoteRegisteredTypes[r];
							bool found = false;
							for (size_t m = 0; m < registeredTypes.size(); m++)
							{
								const bool localMatch = registeredTypes[m].local.name == remoteTypeInfo.remote.name;
								const bool remoteMatch = registeredTypes[m].remote.name == remoteTypeInfo.local.name;
								if (localMatch || remoteMatch)
								{
									if (localMatch && remoteMatch)
									{
										found = true;
										if (registeredTypes[m].local.typeId != remoteTypeInfo.remote.typeId)
											failureString += "\nLocal type '" + registeredTypes[m].local.name + "' typeid mismatch, my local typeid '" + registeredTypes[m].local.typeId + ", remote sync manager's remote typeId '" + remoteTypeInfo.remote.typeId;
										if (registeredTypes[m].remote.typeId != remoteTypeInfo.local.typeId)
											failureString += "\nRemote type '" + registeredTypes[m].remote.name + "' typeid mismatch, my remote typeid '" + registeredTypes[m].remote.typeId + ", remote sync manager's local typeId '" + remoteTypeInfo.local.typeId;
										if (registeredTypes[m].local.version != remoteTypeInfo.remote.version)
											failureString += "\nLocal type '" + registeredTypes[m].local.name + "' version mismatch, my local version '" + std::to_string(registeredTypes[m].local.version) + "', remote sync manager's remote version '" + std::to_string(remoteTypeInfo.remote.version) + "'.";
										if (registeredTypes[m].remote.version != remoteTypeInfo.local.version)
											failureString += "\nRemote type '" + registeredTypes[m].remote.name + "' version mismatch, my remote version '" + std::to_string(registeredTypes[m].remote.version) + "', remote sync manager's local version '" + std::to_string(remoteTypeInfo.local.version) + "'.";
									}
									else if (!localMatch)
										failureString += "\nRemote type names match: '" + remoteTypeInfo.local.name + "', but local names mismatch: my '" + registeredTypes[m].local.name + "' remote: '" + remoteTypeInfo.remote.name + "'!";
									else if (!remoteMatch)
										failureString += "\nLocal type names match: '" + remoteTypeInfo.remote.name + "', but remote names mismatch: my '" + registeredTypes[m].remote.name + "' remote: '" + remoteTypeInfo.local.name + "'!";
									break;
								}
							}
							if (!found)
								failureString += "\nUnknown type: local '" + remoteTypeInfo.remote.name + "', remote '" + remoteTypeInfo.local.name + "'!";
						}
						if (failureString.size() > 1)
						{
							remoteManager.typeCompatibility = TypeCompatibility::incompatible;
							se::log::info(failureString);
						}

						//Write a response packet
						WriteBuffer response;
						response.write(PacketType::types);
						response.write(syncManagerInitializeTypesResponseMagicNumber);
						const TypeCompatibility val = remoteManager.typeCompatibility.load();
						response.write(val);
						socket.sendPacket(response);
					}
					else if (magic == syncManagerInitializeTypesResponseMagicNumber)
					{//Received remote type inspectation results
						TypeCompatibility val;
						buffer.read(val);
						remoteManager.typeCompatibilityResponse = val;
						if (remoteManager.typeCompatibilityResponse == TypeCompatibility::incompatible)
						{
							se::log::info("se::sync::Manager: initialization failed. (my)Local types are not compatible with the remote types.");
						}
					}
					else
					{
						if (debugLevel >= 1)
							se::log::info("se::sync::Manager: received an invalid packet while waiting for the init packet.");
					}
				}
				break;
				case PacketType::create:
				{//The remote sync manager instructs to create a new instance of a local type

					//Entry data
					IType::SyncTypeIdType typeId;
					buffer.read(typeId);
					Entry::Id id;
					buffer.read(id);
					TypeInfo* typeInfo = findByLocalType(typeId);
					if (!typeInfo)
					{
						se::log::warning("se::sync::Manager: PacketType::create had invalid typeId value. The typeId value did not correspond to any known typeId. typeId: " + typeId);
						return false;
					}
					IType* instance = typeInfo->local.constructor();
					if (!instance)
					{
						se::log::warning("se::sync::Manager: PacketType::create local type constructor could not construct an instance. typeId: " + typeId);
						return false;
					}
					Entry* entry = new Entry(*this, instance, id, *typeInfo, false);
					instance->syncCreate(buffer);
					entries.push_back(entry);
					//remoteTypeCreatedSignal(*instance);
				}
				break;
				case PacketType::createResponse:
				{//The remote sync manager has created an entry and its sending its create packet
					Entry::Id id;
					buffer.read(id);
					Entry* entry = findEntry(id);
					if (entry)
					{
						entry->ptr->syncCreate(buffer);
						entry->createReceived = true;
					}
					else
					{
						se::log::error("se::sync::Manager: receive handler: PacketType::createResponse: Unknown entry id!");
						return true;
					}
				}
				break;
				case PacketType::update:
				{//(remote)Local type sends update
					Entry::Id id;
					buffer.read(id);
					Entry* entry = findEntry(id);
					if (entry)
					{
						entry->ptr->syncUpdate(buffer);
					}
					else
					{
						if (debugLevel >= 3)
							se::log::info("se::sync::Manager: receive handler: PacketType::localUpdate: Unknown entry id! Maybe entry was just removed...");
						return true;
					}
				}
				break;
				case PacketType::remove:
				{//(remote)Local type (to be) removed
					Entry::Id id;
					buffer.read(id);
					Entry* entry = findEntry(id);
					if (entry)
					{
						entry->ptr->syncRemove(buffer);
						entry->removeReceived = true;
					}
					else
					{
						se::log::error("se::sync::Manager: receive handler: PacketType::remove: Unknown remote entry id!");
						return true;
					}
				}
				break;
				case PacketType::updateInterval:
				{
					Entry::Id id;
					buffer.read(id);
					se::time::Time interval;
					buffer.read(interval);
					Entry* entry = findEntry(id);
					if (entry)
					{
						entry->interval = interval;
					}
					else
					{
						se::log::error("se::sync::Manager: receive handler: PacketType::updateInterval: Unknown remote entry id!");
						return true;
					}
				}
				break;
				}
			}
			return true;
		}

		bool Manager::isInitialized() const
		{
			return initialized;
		}

		void Manager::update(const se::time::Time& deltaTime)
		{
			se_assert(isInitialized());

			//Local types
			for (size_t i = 0; i < entries.size();)
			{
				se_assert(entries[i] && entries[i]->ptr);

				//Create
				if (!entries[i]->createSent)
				{
					WriteBuffer buffer;
					if (entries[i]->locallyInstantiated)
					{
						buffer.write(PacketType::create);
						buffer.write(entries[i]->typeInfo.remote.typeId);
					}
					else
					{
						buffer.write(PacketType::createResponse);
					}
					buffer.write(entries[i]->id);
					entries[i]->ptr->syncCreate(buffer);
					entries[i]->createSent = socket.sendPacket(buffer);
					if (debugLevel >= 1)
					{
						const TypeInfo* typeInfo = findByLocalType(entries[i]->typeInfo.local.typeId);
						const std::string typeName = typeInfo ? typeInfo->local.name : "unknown";
						const std::string packetName = entries[i]->locallyInstantiated ? "PacketType::create" : "PacketType::createResponse";
						if (entries[i]->createSent)
							se::log::info("se::sync::Manager: " + packetName + " sent for instance of type: " + typeName);
						else
							se::log::info("se::sync::Manager: failed to send " + packetName + " for instance of type: " + typeName);
					}
				}

				//Set update interval
				if (entries[i]->sendRemoteUpdateInterval)
				{
					WriteBuffer buffer;
					buffer.write(Manager::PacketType::updateInterval);
					buffer.write(entries[i]->id);
					buffer.write(entries[i]->requestedRemoteUpdateInterval);
					if (socket.sendPacket(buffer))
						entries[i]->sendRemoteUpdateInterval = false;
				}

				//Update
				entries[i]->timer += deltaTime;
				if (entries[i]->ptr->syncUpdate(deltaTime) || (entries[i]->interval != 0 && entries[i]->timer >= entries[i]->interval))
				{
					entries[i]->timer = 0;
					WriteBuffer buffer;
					buffer.write(PacketType::update);
					buffer.write(entries[i]->id);
					entries[i]->ptr->syncUpdate(buffer);
					const bool updateSent = socket.sendPacket(buffer);
					if (debugLevel >= 3)
					{
						const TypeInfo* typeInfo = findByLocalType(entries[i]->typeInfo.local.typeId);
						const std::string typeName = typeInfo ? typeInfo->local.name : "unknown";
						if (updateSent)
							se::log::info("se::sync::Manager: PacketType::update sent for instance of type: " + typeName);
						else
							se::log::info("se::sync::Manager: failed to send PacketType::update for instance of type: " + typeName);
					}
				}

				if (entries[i]->locallyInstantiated)
				{
					//Remove
					if (entries[i]->handles.empty() && !entries[i]->removeSent)
					{
						WriteBuffer buffer;
						buffer.write(PacketType::remove);
						buffer.write(entries[i]->id);
						entries[i]->ptr->syncRemove(buffer);
						entries[i]->removeSent = socket.sendPacket(buffer);
						if (debugLevel >= 1)
						{
							const TypeInfo* typeInfo = findByLocalType(entries[i]->typeInfo.local.typeId);
							const std::string typeName = typeInfo ? typeInfo->local.name : "unknown";
							if (entries[i]->removeSent)
								se::log::info("se::sync::Manager: PacketType::remove sent for instance of type: " + typeName);
							else
								se::log::info("se::sync::Manager: failed to send PacketType::remove for instance of type: " + typeName);
						}
					}
					//Deallocate after remove packet has been received
					if (entries[i]->removeReceived)
					{
						delete entries[i];
						entries[i] = entries.back();
						entries.pop_back();
					}
					else
						i++;
				}
				else
				{
					//Deallocate
					if (entries[i]->removeReceived && entries[i]->removeSent)
					{
						delete entries[i];
						entries[i] = entries.back();
						entries.pop_back();
					}
					else
						i++;
				}
			}
		}

		TypeInfo* Manager::findByLocalType(const IType::SyncTypeIdType& typeId)
		{
			for (size_t i = 0; i < registeredTypes.size(); i++)
			{
				if (registeredTypes[i].local.typeId == typeId)
					return &registeredTypes[i];
			}
			return nullptr;
		}

		TypeInfo* Manager::findByRemoteType(const IType::SyncTypeIdType& typeId)
		{
			for (size_t i = 0; i < registeredTypes.size(); i++)
			{
				if (registeredTypes[i].remote.typeId == typeId)
					return &registeredTypes[i];
			}
			return nullptr;
		}

		Entry* Manager::findEntry(const Entry::Id id)
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				if (entries[i]->id == id)
					return entries[i];
			}
			return nullptr;
		}
	}
}