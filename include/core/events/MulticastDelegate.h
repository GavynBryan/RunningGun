#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <cstdint>

using DelegateHandle = uint64_t;

/**
 * A type-safe multicast delegate that allows multiple listeners to subscribe to events.
 * Follows the observer pattern with automatic handle management for safe unsubscription.
 *
 * @tparam Args The parameter types that will be passed when the delegate is broadcast.
 */
template<typename... Args>
class MulticastDelegate
{
private:
	struct Subscription
	{
		DelegateHandle Handle;
		std::function<void(Args...)> Callback;
	};

	std::vector<Subscription> Subscriptions;
	DelegateHandle NextHandle = 1;

public:
	MulticastDelegate() = default;
	~MulticastDelegate() = default;

	MulticastDelegate(const MulticastDelegate&) = delete;
	MulticastDelegate& operator=(const MulticastDelegate&) = delete;

	MulticastDelegate(MulticastDelegate&&) = default;
	MulticastDelegate& operator=(MulticastDelegate&&) = default;

	/**
	 * Subscribe a callback to this delegate.
	 * @param _callback The function to call when the delegate is broadcast.
	 * @return A handle that can be used to unsubscribe later.
	 */
	DelegateHandle Subscribe(std::function<void(Args...)> _callback)
	{
		DelegateHandle _handle = NextHandle++;
		Subscriptions.push_back({ _handle, std::move(_callback) });
		return _handle;
	}

	/**
	 * Unsubscribe a previously subscribed callback using its handle.
	 * @param _handle The handle returned from Subscribe().
	 */
	void Unsubscribe(DelegateHandle _handle)
	{
		Subscriptions.erase(
			std::remove_if(Subscriptions.begin(), Subscriptions.end(),
				[_handle](const Subscription& _sub) { return _sub.Handle == _handle; }),
			Subscriptions.end()
		);
	}

	/**
	 * Broadcast the event to all subscribed listeners.
	 * Safe to call Subscribe/Unsubscribe from within callbacks.
	 * @param _args The arguments to pass to each subscribed callback.
	 */
	void Broadcast(Args&&... _args)
	{
		// Copy subscriptions to allow safe modification during iteration
		auto _snapshot = Subscriptions;
		for (const auto& _sub : _snapshot)
		{
			_sub.Callback(std::forward<Args>(_args)...);
		}
	}

	/**
	 * Remove all subscriptions.
	 */
	void Clear()
	{
		Subscriptions.clear();
	}

	/**
	 * Check if there are any active subscriptions.
	 * @return True if at least one callback is subscribed.
	 */
	bool HasSubscribers() const
	{
		return !Subscriptions.empty();
	}

	/**
	 * Get the number of active subscriptions.
	 * @return The count of subscribed callbacks.
	 */
	size_t SubscriberCount() const
	{
		return Subscriptions.size();
	}
};
