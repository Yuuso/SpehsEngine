#pragma once


namespace se
{
	namespace debug
	{
		struct IConnectionManagerVisualizer
		{
			static std::unique_ptr<IConnectionManagerVisualizer> create(net::ConnectionManager& ConnectionManager, imgui::BackendWrapper& imGuiBackendWrapper, const bool enabled);
			virtual ~IConnectionManagerVisualizer() = default;
			virtual void setEnabled(const bool enabled) = 0;
			virtual bool getEnabled() const = 0;
		};
	}
}
