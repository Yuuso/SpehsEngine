#pragma once


namespace se::gfx
{
	enum class ShaderVariant
	{
		Default,
		Instanced,
		Skinned,
		SkinnedInstanced,
		Billboard,
		BillboardInstanced,
	};

	static inline std::string toString(ShaderVariant _variant)
	{
		switch (_variant)
		{
			case ShaderVariant::Default:			return "Default";
			case ShaderVariant::Instanced:			return "Instanced";
			case ShaderVariant::Skinned:			return "Skinned";
			case ShaderVariant::SkinnedInstanced:	return "SkinnedInstanced";
			case ShaderVariant::Billboard:			return "Billboard";
			case ShaderVariant::BillboardInstanced:	return "BillboardInstanced";
		}
		return "Unknown";
	}

	static inline ShaderVariant getShaderVariant(const bool _instanced, const bool _skinned, const bool _billboard)
	{
		if (_skinned)
		{
			if (_instanced)
			{
				return ShaderVariant::SkinnedInstanced;
			}
			else
			{
				return ShaderVariant::Skinned;
			}
		}
		else if (_billboard)
		{
			if (_instanced)
			{
				return ShaderVariant::BillboardInstanced;
			}
			else
			{
				return ShaderVariant::Billboard;
			}
		}
		else if (_instanced)
		{
			return ShaderVariant::Instanced;
		}
		return ShaderVariant::Default;
	}
}
