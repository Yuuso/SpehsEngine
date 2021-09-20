#pragma once


namespace se
{
	namespace graphics
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
}
