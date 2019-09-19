#pragma once
class LightType
{
public:

	enum Type {
		ParallelLight,
		PointLight,
		SpotLight
	};

	LightType();
	~LightType();
	static Type Get() { return s_LightType; };
	static void Set(Type type) { s_LightType = type; }

private:
	static Type s_LightType;
};
