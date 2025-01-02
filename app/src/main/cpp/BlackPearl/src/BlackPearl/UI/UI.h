#pragma once
namespace BlackPearl {


	struct AccelerationStructureUIData
	{
		// Instance settings (no rebuild required)
		bool                                ForceOpaque = false;

		// BVH settings (require rebuild to take effect)
		bool                                ExcludeTransmissive = false;

		bool                                IsDirty = false;
	};


	struct UIData
	{
		AccelerationStructureUIData         AS;
	};


};