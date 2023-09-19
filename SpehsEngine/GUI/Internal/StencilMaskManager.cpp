#include "stdafx.h"
#include "SpehsEngine/GUI/Internal/StencilMaskManager.h"

#include <vector>

using namespace se::gfx;


namespace se::gui
{
	class StencilMaskDepthImpl : public StencilMaskManager
	{
	public:
		void begin() override;
		void end() override;
		bool enabled() const override;
		Stencil get(bool _write) const override;

	private:
		uint32_t currentLayer = 0;
	};

	class StencilMaskIncrementingImpl : public StencilMaskManager
	{
	public:
		void begin() override;
		void end() override;
		bool enabled() const override;
		Stencil get(bool _write) const override;

	private:
		std::vector<uint32_t> activeLayers;
		uint32_t nextNewLayer = 1;
	};

	class StencilMaskBitMaskImpl : public StencilMaskManager
	{
	public:
		void begin() override;
		void end() override;
		bool enabled() const override;
		Stencil get(bool _write) const override;

	private:
		std::vector<uint32_t> activeBits;
		uint32_t nextBit = 0;
	};


	std::unique_ptr<StencilMaskManager> createStencilMaskManager(LayerMaskStyle _style)
	{
		switch (_style)
		{
			case LayerMaskStyle::Depth:			return std::make_unique<StencilMaskDepthImpl>();
			case LayerMaskStyle::Incrementing:	return std::make_unique<StencilMaskIncrementingImpl>();
			case LayerMaskStyle::BitMask:		return std::make_unique<StencilMaskBitMaskImpl>();
		}
		return nullptr;
	}


	void StencilMaskDepthImpl::begin()
	{
		currentLayer++;
		se_assert_m(currentLayer <= 0xffu, "LayerMaskStyle::Depth supports at most 255 layers of masking.");
	}
	void StencilMaskDepthImpl::end()
	{
		currentLayer--;
	}
	bool StencilMaskDepthImpl::enabled() const
	{
		return currentLayer > 0;
	}
	Stencil StencilMaskDepthImpl::get(bool _write) const
	{
		se_assert(currentLayer > 0);

		Stencil stencil;
		stencil.maskValue = 0xffu;
		stencil.test = StencilTest::Equal;
		stencil.stencilFail = StencilOperation::Keep;

		if (_write)
		{
			stencil.refValue = std::min(0xffu, currentLayer - 1);
			stencil.depthFail = StencilOperation::Increment;
			stencil.pass = StencilOperation::Increment;
		}
		else
		{
			stencil.refValue = std::min(0xffu, currentLayer);
			stencil.depthFail = StencilOperation::Keep;
			stencil.pass = StencilOperation::Keep;
		}

		return stencil;
	}

	void StencilMaskIncrementingImpl::begin()
	{
		if (activeLayers.empty())
		{
			activeLayers.push_back(nextNewLayer++);
		}
		else
		{
			activeLayers.push_back(activeLayers.back() + 1u);
			se_assert(activeLayers.back() <= nextNewLayer);
			if (activeLayers.back() == nextNewLayer)
			{
				nextNewLayer++;
			}
		}
		se_assert_m(activeLayers.back() <= 0xffu, "LayerMaskStyle::Incrementing supports at most 255 elements with layer masking + any number of shared masking children.");
	}
	void StencilMaskIncrementingImpl::end()
	{
		activeLayers.pop_back();
	}
	bool StencilMaskIncrementingImpl::enabled() const
	{
		return !activeLayers.empty();
	}
	Stencil StencilMaskIncrementingImpl::get(bool _write) const
	{
		se_assert(!activeLayers.empty());

		Stencil stencil;

		stencil.maskValue = 0xffu;
		stencil.stencilFail = StencilOperation::Keep;

		if (_write && activeLayers.size() == 1)
		{
			stencil.test = StencilTest::Always;
			stencil.refValue = std::min(0xffu, activeLayers.back());
			stencil.depthFail = StencilOperation::Replace;
			stencil.pass = StencilOperation::Replace;
		}
		else
		{
			stencil.test = StencilTest::Equal;

			if (_write)
			{
				stencil.refValue = std::min(0xffu, activeLayers.back() - 1);
				stencil.depthFail = StencilOperation::Increment;
				stencil.pass = StencilOperation::Increment;
			}
			else
			{
				stencil.refValue = std::min(0xffu, activeLayers.back());
				stencil.depthFail = StencilOperation::Keep;
				stencil.pass = StencilOperation::Keep;
			}
		}

		return stencil;
	}

	void StencilMaskBitMaskImpl::begin()
	{
		const uint32_t currentBit = 1 << nextBit++;
		activeBits.push_back(currentBit);
		se_assert_m(currentBit <= 0xffu, "LayerMaskStyle::BitMask supports only 8 elements with masking.");
	}
	void StencilMaskBitMaskImpl::end()
	{
		activeBits.pop_back();
	}
	bool StencilMaskBitMaskImpl::enabled() const
	{
		return !activeBits.empty();
	}
	Stencil StencilMaskBitMaskImpl::get(bool _write) const
	{
		se_assert(!activeBits.empty());

		Stencil stencil;

		stencil.maskValue = 0;
		for (auto&& bit : activeBits)
		{
			enableBit(stencil.maskValue, bit);
		}

		stencil.refValue = stencil.maskValue;
		stencil.test = StencilTest::Equal;
		stencil.stencilFail = StencilOperation::Keep;

		if (_write)
		{
			disableBit(stencil.maskValue, activeBits.back());
			stencil.depthFail = StencilOperation::Replace;
			stencil.pass = StencilOperation::Replace;
		}
		else
		{
			stencil.depthFail = StencilOperation::Keep;
			stencil.pass = StencilOperation::Keep;
		}

		return stencil;
	}
}
