#pragma once

#include "SpehsEngine/Core/FunctionTraits.h"
#include <vector>
#include <utility>


namespace se
{
	class ScopedConnection;
	namespace detail
	{
		constexpr size_t InactiveCallIndex = ~0u;
		constexpr size_t ResetScopedConnectionIndex = 0xDDDDDDDDDDDDDDDD;

		class SignalBase
		{
		public:
			SignalBase() = default;
			SignalBase(const SignalBase& _copy) = delete;
			void operator=(const SignalBase& _copy) = delete;
			inline SignalBase(SignalBase&& _move);
			inline void operator=(SignalBase&& _move);
			inline virtual ~SignalBase();
			inline bool isEmpty() const;
			inline size_t getConnectionCount() const;
			inline virtual void disconnectAllConnections();
		protected:
			friend class ScopedConnection;
			inline void insertFrontScopedConnection(ScopedConnection& _scopedConnection);
			inline void pushBackScopedConnection(ScopedConnection& _scopedConnection);
			inline void popBackScopedConnection();
			inline void swapOutScopedConnection(const size_t _index);
			inline void eraseScopedConnection(const size_t _index);
			virtual inline void removeScopedConnection(const size_t _index) = 0;
			virtual inline bool onConnect() = 0;
			std::vector<ScopedConnection*> scopedConnections;
		};
	}

	class ScopedConnection
	{
	public:
		ScopedConnection() = default;
		inline ~ScopedConnection();
		inline ScopedConnection(ScopedConnection&& _move);
		inline ScopedConnection& operator=(ScopedConnection&& _move);
		ScopedConnection(const ScopedConnection& _copy) = delete;
		ScopedConnection& operator=(const ScopedConnection& _copy) = delete;
		inline explicit operator bool() const { return signal != nullptr; }
		inline void swap(ScopedConnection& _other);
		inline void disconnect();
		inline bool isConnected() const;
	private:
		friend class detail::SignalBase;
		detail::SignalBase* signal = nullptr;
		size_t index = detail::ResetScopedConnectionIndex;
	};

	namespace detail
	{
		template<typename Signature>
		struct SignalCallResult
		{
			typedef typename std::conditional<std::is_same<typename FunctionTraits<Signature>::ResultType, void>::value,
				void, std::optional<typename FunctionTraits<Signature>::ResultType>>::type type;
		};

		// After the call, return the temporarily moved function back to the functions vector
		template<typename Signature>
		inline void postCallSignal(std::vector<std::function<Signature>>& _functions, std::function<Signature>&& _function, const size_t _likelyIndex)
		{
			const size_t functionCount = _functions.size();
			if (_likelyIndex < functionCount)
			{
				// Try to quickly locate the empty index
				if (!_functions[_likelyIndex])
				{
					std::swap(_function, _functions[_likelyIndex]);
					return;
				}
			}
			// Search everything
			for (size_t i = 0; i < functionCount; i++)
			{
				if (!_functions[i])
				{
					std::swap(_function, _functions[i]);
					return;
				}
			}
		}

		template<typename Signature, typename ... Args>
		inline typename SignalCallResult<Signature>::type callSignal(std::vector<std::function<Signature>>& _functions, size_t& _callIndex, Args&& ... _args)
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			se_assert(_callIndex == detail::InactiveCallIndex);
			_callIndex = 0;
			if constexpr (std::is_same<FunctionResultType, void>::value)
			{
				while (_callIndex < _functions.size())
				{
					std::function<Signature> function;
					std::swap(function, _functions[_callIndex++]);
					function(std::forward<Args&&>(_args)...);
					postCallSignal(_functions, std::move(function), _callIndex);
				}
				_callIndex = detail::InactiveCallIndex;
			}
			else
			{
				std::optional<FunctionResultType> result;
				while (_callIndex < _functions.size())
				{
					std::function<Signature> function;
					std::swap(function, _functions[_callIndex++]);
					result = function(std::forward<Args&&>(_args)...);
					postCallSignal(_functions, std::move(function), _callIndex);
				}
				_callIndex = detail::InactiveCallIndex;
				return result;
			}
		}

		template<typename Signature, size_t arity> struct signalN {};
		template<typename Signature> struct signalN<Signature, 0>
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			typedef typename SignalCallResult<Signature>::type OptionalResultType;
			inline OptionalResultType operator()()
			{
				if constexpr (std::is_same<FunctionResultType, void>::value)
					callSignal<Signature>(functions, callIndex);
				else
					return callSignal<Signature>(functions, callIndex);
			}
			std::vector<std::function<Signature>> functions;
			size_t callIndex = detail::InactiveCallIndex;
		};
		template<typename Signature> struct signalN<Signature, 1>
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			typedef typename SignalCallResult<Signature>::type OptionalResultType;
			inline OptionalResultType operator()(
				FunctionTraits<Signature>::Arg1Type _1)
			{
				if constexpr (std::is_same<FunctionResultType, void>::value)
					callSignal<Signature>(functions, callIndex, _1);
				else
					return callSignal<Signature>(functions, callIndex, _1);
			}
			std::vector<std::function<Signature>> functions;
			size_t callIndex = detail::InactiveCallIndex;
		};
		template<typename Signature> struct signalN<Signature, 2>
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			typedef typename SignalCallResult<Signature>::type OptionalResultType;
			inline OptionalResultType operator()(
				FunctionTraits<Signature>::Arg1Type _1,
				FunctionTraits<Signature>::Arg2Type _2)
			{
				if constexpr (std::is_same<FunctionResultType, void>::value)
					callSignal<Signature>(functions, callIndex, _1, _2);
				else
					return callSignal<Signature>(functions, callIndex, _1, _2);
			}
			std::vector<std::function<Signature>> functions;
			size_t callIndex = detail::InactiveCallIndex;
		};
		template<typename Signature> struct signalN<Signature, 3>
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			typedef typename SignalCallResult<Signature>::type OptionalResultType;
			inline OptionalResultType operator()(
				FunctionTraits<Signature>::Arg1Type _1,
				FunctionTraits<Signature>::Arg2Type _2,
				FunctionTraits<Signature>::Arg3Type _3)
			{
				if constexpr (std::is_same<FunctionResultType, void>::value)
					callSignal<Signature>(functions, callIndex, _1, _2, _3);
				else
					return callSignal<Signature>(functions, callIndex, _1, _2, _3);
			}
			std::vector<std::function<Signature>> functions;
			size_t callIndex = detail::InactiveCallIndex;
		};
		template<typename Signature> struct signalN<Signature, 4>
		{
			typedef typename FunctionTraits<Signature>::ResultType FunctionResultType;
			typedef typename SignalCallResult<Signature>::type OptionalResultType;
			inline OptionalResultType operator()(
				FunctionTraits<Signature>::Arg1Type _1,
				FunctionTraits<Signature>::Arg2Type _2,
				FunctionTraits<Signature>::Arg3Type _3,
				FunctionTraits<Signature>::Arg4Type _4)
			{
				if constexpr (std::is_same<FunctionResultType, void>::value)
					callSignal<Signature>(functions, callIndex, _1, _2, _3, _4);
				else
					return callSignal<Signature>(functions, callIndex, _1, _2, _3, _4);
			}
			std::vector<std::function<Signature>> functions;
			size_t callIndex = detail::InactiveCallIndex;
		};
	}

	/*
		1.	This implementation is NOT thread safe.
		2.	A signal call must not result in the deletion of the calling signal object.
		3.	A signal should not be called again from within an already executing signal call.
			Unfortunately writing such code cannot be detected at compile time
			so if the user designs such code an assert can only be fired at run time.
		4.	Scoped connections to a signal are allowed to be added and removed during a signal call.
			If a connection is removed before its turn on the call, the call will not be made.
			If a connection is added during a call, the call will not be made.
	*/
	template<typename Signature>
	struct Signal final : public detail::SignalBase, public detail::signalN<Signature, FunctionTraits<Signature>::arity>
	{
		typedef Signal<Signature> SignalType;

		constexpr Signal() = default;
		~Signal()
		{
			se_assert(this->callIndex == detail::InactiveCallIndex); // Must not destroy signal from within its call
		}

		inline Signal(SignalType&& _move)
			: detail::SignalBase(std::move(_move))
		{
			se_assert(_move.callIndex == detail::InactiveCallIndex);
			std::swap(this->functions, _move.functions);
			std::swap(this->callIndex, _move.callIndex);
		}

		inline SignalType& operator=(SignalType&& _move)
		{
			se_assert(_move.callIndex == detail::InactiveCallIndex);
			se_assert(this->callIndex == detail::InactiveCallIndex);
			detail::SignalBase::operator=(std::move(_move));
			std::swap(this->functions, _move.functions);
			std::swap(this->callIndex, _move.callIndex);
			return *this;
		}

		inline ScopedConnection connect(const std::function<Signature>& _function)
		{
			ScopedConnection scopedConnection;
			if (onConnect())
			{
				// Push to front, connections established during call will not be executed on the same call
				insertFrontScopedConnection(scopedConnection);
				this->functions.insert(this->functions.begin(), _function);
			}
			else
			{
				pushBackScopedConnection(scopedConnection);
				this->functions.push_back(_function);
			}
			return scopedConnection;
		}

		inline void disconnectAllConnections() final
		{
			se_assert(this->callIndex == detail::InactiveCallIndex);
			detail::SignalBase::disconnectAllConnections();
			this->functions.clear();
			this->callIndex = detail::InactiveCallIndex;
		}

	private:

		// Called from ScopedConnection that resets its own signal pointer.
		inline void removeScopedConnection(const size_t _index) final
		{
			if ((_index + 1) == this->scopedConnections.size())
			{
				// Simple pop back
				this->functions.pop_back();
				popBackScopedConnection();
			}
			else if (this->callIndex == detail::InactiveCallIndex || _index >= this->callIndex)
			{
				// No ongoing active call or index is among the upcoming indices -> swap out
				std::swap(this->functions[_index], this->functions.back());
				this->functions.pop_back();
				swapOutScopedConnection(_index);
			}
			else
			{
				// Removed already processed during call, must use erase
				this->callIndex--;
				this->functions.erase(this->functions.begin() + _index);
				eraseScopedConnection(_index);
			}
		}

		inline bool onConnect() final
		{
			if (this->callIndex == detail::InactiveCallIndex)
			{
				return false;
			}
			else
			{
				this->callIndex++;
				return true;
			}
		}
	};

	inline ScopedConnection::~ScopedConnection()
	{
		disconnect();
	}

	inline ScopedConnection::ScopedConnection(ScopedConnection&& _move)
	{
		if (_move.signal)
		{
			signal = _move.signal;
			index = _move.index;
			_move.signal->scopedConnections[_move.index] = this;
			_move.signal = nullptr;
			_move.index = detail::ResetScopedConnectionIndex;
		}
	}

	inline ScopedConnection& ScopedConnection::operator=(ScopedConnection&& _move)
	{
		disconnect();
		if (_move.signal)
		{
			signal = _move.signal;
			index = _move.index;
			_move.signal->scopedConnections[_move.index] = this;
			_move.signal = nullptr;
			_move.index = detail::ResetScopedConnectionIndex;
		}
		return *this;
	}

	inline void ScopedConnection::swap(ScopedConnection& _other)
	{
		detail::SignalBase* const oldSignal = signal;
		const size_t oldIndex = index;
		signal = _other.signal;
		if (_other.signal)
		{
			se_assert(_other.signal->scopedConnections[_other.index] == &_other);
			_other.signal->scopedConnections[_other.index] = this;
			index = _other.index;
		}
		_other.signal = oldSignal;
		if (oldSignal)
		{
			se_assert(oldSignal->scopedConnections[oldIndex] == this);
			oldSignal->scopedConnections[oldIndex] = &_other;
			_other.index = oldIndex;
		}
	}

	inline void ScopedConnection::disconnect()
	{
		if (signal)
		{
			signal->removeScopedConnection(index);
			se_assert(!signal);
			se_assert(index == detail::ResetScopedConnectionIndex);
		}
	}

	inline bool ScopedConnection::isConnected() const
	{
		return signal != nullptr;
	}

	namespace detail
	{
		inline SignalBase::SignalBase(SignalBase&& _move)
		{
			for (ScopedConnection* const scopedConnection : scopedConnections)
			{
				se_assert(scopedConnection->signal == this);
				scopedConnection->signal = &_move;
			}
			for (ScopedConnection* const scopedConnection : _move.scopedConnections)
			{
				se_assert(scopedConnection->signal == &_move);
				scopedConnection->signal = this;
			}
			std::swap(scopedConnections, _move.scopedConnections);
		}

		inline void SignalBase::operator=(SignalBase&& _move)
		{
			for (ScopedConnection* const scopedConnection : scopedConnections)
			{
				se_assert(scopedConnection->signal == this);
				scopedConnection->signal = &_move;
			}
			for (ScopedConnection* const scopedConnection : _move.scopedConnections)
			{
				se_assert(scopedConnection->signal == &_move);
				scopedConnection->signal = this;
			}
			std::swap(scopedConnections, _move.scopedConnections);
		}

		inline SignalBase::~SignalBase()
		{
			for (size_t i = 0; i < scopedConnections.size(); i++)
			{
				ScopedConnection& scopedConnection = *scopedConnections[i];
				se_assert(scopedConnection.signal == this);
				se_assert(scopedConnection.index == i);
				scopedConnection.signal = nullptr;
				scopedConnection.index = ResetScopedConnectionIndex;
			}
		}

		inline bool SignalBase::isEmpty() const
		{
			return scopedConnections.empty();
		}

		inline size_t SignalBase::getConnectionCount() const
		{
			return scopedConnections.size();
		}

		inline void SignalBase::disconnectAllConnections()
		{
			for (size_t i = 0; i < scopedConnections.size(); i++)
			{
				ScopedConnection& scopedConnection = *scopedConnections[i];
				se_assert(scopedConnection.signal == this);
				se_assert(scopedConnection.index == i);
				scopedConnection.signal = nullptr;
				scopedConnection.index = ResetScopedConnectionIndex;
			}
			scopedConnections.clear();
		}

		inline void SignalBase::insertFrontScopedConnection(ScopedConnection& _scopedConnection)
		{
			for (size_t i = 0; i < scopedConnections.size(); i++)
			{
				se_assert(scopedConnections[i]->index == i);
				scopedConnections[i]->index++;
			}
			_scopedConnection.signal = this;
			_scopedConnection.index = 0;
			scopedConnections.insert(scopedConnections.begin(), &_scopedConnection);
		}

		inline void SignalBase::pushBackScopedConnection(ScopedConnection& _scopedConnection)
		{
			_scopedConnection.signal = this;
			_scopedConnection.index = scopedConnections.size();
			scopedConnections.push_back(&_scopedConnection);
		}

		inline void SignalBase::popBackScopedConnection()
		{
			scopedConnections.back()->signal = nullptr;
			scopedConnections.back()->index = ResetScopedConnectionIndex;
			scopedConnections.pop_back();
		}

		inline void SignalBase::swapOutScopedConnection(const size_t _index)
		{
			se_assert((_index + 1) < scopedConnections.size());
			std::swap(scopedConnections[_index], scopedConnections.back());
			scopedConnections[_index]->index = _index;
			scopedConnections.back()->signal = nullptr;
			scopedConnections.back()->index = ResetScopedConnectionIndex;
			scopedConnections.pop_back();
		}

		inline void SignalBase::eraseScopedConnection(const size_t _index)
		{
			se_assert((_index + 1) < scopedConnections.size());
			scopedConnections[_index]->signal = nullptr;
			scopedConnections[_index]->index = ResetScopedConnectionIndex;
			scopedConnections.erase(scopedConnections.begin() + _index);
			for (size_t i = _index; i < scopedConnections.size(); i++)
			{
				se_assert(scopedConnections[i]->index == (i + 1));
				scopedConnections[i]->index--;
			}
		}
	}
}